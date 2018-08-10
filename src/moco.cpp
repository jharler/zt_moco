/**************************************************************************************************
** file: src\moco.cpp
**
** Primary functionality of moco (mo)del (co)nverter
**************************************************************************************************/

// headers ========================================================================================================================================================================================

#define ZT_GAME_INTERNAL_DECLARATIONS

#include "moco.h"

#pragma warning(push)
#pragma warning(disable : 4530)
#include "../build/libs/assimp-4.1.0/include/assimp/cimport.h"
#include "../build/libs/assimp-4.1.0/include/assimp/scene.h"
#include "../build/libs/assimp-4.1.0/include/assimp/postprocess.h"
#pragma warning(pop)

#pragma comment(lib, "assimp-vc120-mt.lib")

// variables ======================================================================================================================================================================================

ztInternal const char *_mocoErrorStrings[MocoErrorType_MAX] = {
	"No Error",
	"Input File Not Found",
	"Assimp aiImportFile function failed",
};

// private functions ==============================================================================================================================================================================

void _aiLogStreamCallback(const char* message, char* user)
{
	if (zt_strFindPos(message, "%", 0) != ztStrPosNotFound) {
		char copy[2048];
		zt_strCpy(copy, zt_elementsOf(copy), message);

		int s_len = zt_strLen(copy);

		zt_fiz(s_len) {
			if (copy[i] == '%') {
				copy[i] = '$';
			}
		}
		zt_logInfo(copy);
	}
	else {
		zt_logInfo(message);
	}
}

// ================================================================================================================================================================================================

ztMat4 zt_mat4(aiMatrix4x4 matrix)
{
	return zt_mat4(matrix.a1, matrix.b1, matrix.c1, matrix.d1, matrix.a2, matrix.b2, matrix.c2, matrix.d2, matrix.a3, matrix.b3, matrix.c3, matrix.d3, matrix.a4, matrix.b4, matrix.c4, matrix.d4);
}

// ================================================================================================================================================================================================

bool zt_serialWriteMat4(ztSerial *serial, aiMatrix4x4 matrix, bool scale_to_one, bool from_blender)
{
	aiVector3D position;
	aiQuaternion rotation;
	aiVector3D scaling;

	matrix.Decompose(scaling, rotation, position);

	if (scale_to_one) {
		scaling = aiVector3D(1, 1, 1);
	}

	if (from_blender) {
		ztQuat quat = zt_quat(rotation.x, rotation.y, rotation.z, rotation.w);
		ztVec3 euler = quat.euler();
		rotation = aiQuaternion(zt_degreesToRadians(euler.y), zt_degreesToRadians(euler.z), zt_degreesToRadians(euler.x + 90));
	}

	if (!zt_serialWrite(serial, zt_vec3(position.x, position.y, position.z)) ||
		!zt_serialWrite(serial, zt_quat(rotation.x, rotation.y, rotation.z, rotation.w)) ||
		!zt_serialWrite(serial, zt_vec3(scaling.x, scaling.y, scaling.z))) {
		return false;
	}

	return true;
}

// ================================================================================================================================================================================================

struct MocoBone
{
	char        name[128];
	aiMatrix4x4 offset;
	i32         parent_idx;
	i32         sorted_idx;

	MocoBone   *first_child;
	MocoBone   *next;
	MocoBone   *parent;
};

// ================================================================================================================================================================================================

ztInternal int _mocoFindBoneParentIndex(const aiNode *scene_root, const aiNode *root, const char *bone_name, MocoBone *bones, int bones_size, int *bones_idx)
{
	zt_fiz(root->mNumChildren) {
		if (zt_strEquals(root->mChildren[i]->mName.C_Str(), bone_name)) {
			// find the bone index that matches the root name

			zt_fjz(*bones_idx) {
				if (zt_strEquals(bones[j].name, root->mName.C_Str())) {
					return j;
				}
			}

			i32 parent_idx = _mocoFindBoneParentIndex(scene_root, scene_root, root->mName.C_Str(), bones, bones_size, bones_idx);
			if (parent_idx != -1) {
				zt_assert(*bones_idx < bones_size);
				int idx = (*bones_idx)++;
				zt_strCpy(bones[idx].name, zt_elementsOf(bones[idx].name), root->mName.C_Str());
				bones[idx].offset = bones[parent_idx].offset;
				bones[idx].parent_idx = parent_idx;

				return idx;
			}

			return -1;
		}
	}

	// didn't find a matching child name in this node, check further down the hierarchy
	zt_fiz(root->mNumChildren) {
		i32 parent = _mocoFindBoneParentIndex(scene_root, root->mChildren[i], bone_name, bones, bones_size, bones_idx);
		if (parent != -1) {
			return parent;
		}
	}

	return -1; // didn't find any matching bones
}

// ================================================================================================================================================================================================

// functions ======================================================================================================================================================================================

int mocoGetErrorMessage(MocoErrorType_Enum error_type, char *buffer, int buffer_size)
{
	return zt_strCpy(buffer, buffer_size, _mocoErrorStrings[error_type]);
}

// ================================================================================================================================================================================================

bool mocoConvertFile(MocoConvertOptions *options, const char *file_in, const char *file_out, MocoErrorType_Enum *error)
{
	if (!zt_fileExists(file_in)) {
		if (error) *error = MocoErrorType_InputFileNotFound;
		return false;
	}

	aiLogStream log_stream;
	log_stream.callback = _aiLogStreamCallback;
	log_stream.user = nullptr;

	//aiAttachLogStream(&log_stream);
	//aiEnableVerboseLogging(true);

	const aiScene *scene = aiImportFile(file_in, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FindDegenerates | aiProcess_FlipUVs);

	//aiDetachLogStream(&log_stream);

	if (scene == nullptr) {
		if (error) *error = MocoErrorType_aiImportFileFailed;
		return false;
	}

	bool needs_transform = options && options->apply_root_transform; //options->root_transform != ztMat4::identity;
	bool from_blender = options && options->from_blender;

	// populate bones
	int       bones_size = 1024;
	int       bones_count = 0;
	MocoBone *bones = zt_mallocStructArray(MocoBone, bones_size);
	MocoBone *root_bone = nullptr;
	MocoBone *native_root_bone = nullptr;

	aiVector3D root_bone_scale_multiplier = aiVector3D(1, 1, 1);

	{
		zt_fiz(scene->mNumMeshes) {
			zt_fjz(scene->mMeshes[i]->mNumBones) {
				aiBone *bone = scene->mMeshes[i]->mBones[j];

				bool found = false;
				zt_fkz(bones_count) {
					if (zt_strEquals(bones[k].name, bone->mName.C_Str())) {

						zt_assert(
							zt_real32Close(bones[k].offset.a1, bone->mOffsetMatrix.a1) && zt_real32Close(bones[k].offset.a2, bone->mOffsetMatrix.a2) && zt_real32Close(bones[k].offset.a3, bone->mOffsetMatrix.a3) && zt_real32Close(bones[k].offset.a4, bone->mOffsetMatrix.a4) &&
							zt_real32Close(bones[k].offset.b1, bone->mOffsetMatrix.b1) && zt_real32Close(bones[k].offset.b2, bone->mOffsetMatrix.b2) && zt_real32Close(bones[k].offset.b3, bone->mOffsetMatrix.b3) && zt_real32Close(bones[k].offset.b4, bone->mOffsetMatrix.b4) &&
							zt_real32Close(bones[k].offset.c1, bone->mOffsetMatrix.c1) && zt_real32Close(bones[k].offset.c2, bone->mOffsetMatrix.c2) && zt_real32Close(bones[k].offset.c3, bone->mOffsetMatrix.c3) && zt_real32Close(bones[k].offset.c4, bone->mOffsetMatrix.c4) &&
							zt_real32Close(bones[k].offset.d1, bone->mOffsetMatrix.d1) && zt_real32Close(bones[k].offset.d2, bone->mOffsetMatrix.d2) && zt_real32Close(bones[k].offset.d3, bone->mOffsetMatrix.d3) && zt_real32Close(bones[k].offset.d4, bone->mOffsetMatrix.d4));

						found = true;
						break;
					}
				}

				if (!found) {
					int idx = bones_count++;
					zt_strCpy(bones[idx].name, zt_elementsOf(bones[idx].name), bone->mName.C_Str());
					bones[idx].offset = bone->mOffsetMatrix;
					bones[idx].parent_idx = -1;

					aiVector3D position;
					aiQuaternion rotation;

					bone->mOffsetMatrix.Decompose(root_bone_scale_multiplier, rotation, position);
				}
			}
		}

		if (bones_count) {
			zt_fiz(bones_count) {
				bones[i].parent_idx = _mocoFindBoneParentIndex(scene->mRootNode, scene->mRootNode, bones[i].name, bones, bones_size, &bones_count);
			}

			// need to add "Root" bone
			zt_assert(bones_count < bones_size);
			int root_idx = bones_count++;
			zt_strCpy(bones[root_idx].name, zt_elementsOf(bones[root_idx].name), "Root");
			bones[root_idx].offset = aiMatrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
			zt_assert(bones[root_idx].offset.IsIdentity());

			if (options && options->from_blender) {
				bones[root_idx].offset = aiMatrix4x4(aiVector3D(1, 1, 1), aiQuaternion(0, 0, zt_degreesToRadians(90)), aiVector3D(0, 0, 0));
			}

			bones[root_idx].parent_idx = -1;

			// populate bone hierarchy
			zt_fiz(bones_count - 1) {
				if (bones[i].parent_idx == -1) {
					bones[i].parent_idx = root_idx;
					native_root_bone = &bones[i];
				}

				MocoBone *parent = &bones[bones[i].parent_idx];
				MocoBone *child = &bones[i];
				zt_singleLinkAddToEnd(parent->first_child, child);
				child->parent = parent;
			}

			// sort everything hierarchically

			struct Bones
			{
				static void sort(MocoBone *bone, int *sorted_bones_idx)
				{
					bone->sorted_idx = (*sorted_bones_idx)++;

					zt_flink(child, bone->first_child) {
						sort(child, sorted_bones_idx);
					}
				}
			};

			root_bone = &bones[root_idx];

			int sorted_bones_idx = 0;
			Bones::sort(root_bone, &sorted_bones_idx);

			zt_assert(sorted_bones_idx == bones_count);
		}
	}

	int meshes_to_skip = 0;
	zt_fiz(scene->mNumMeshes) {
		if (scene->mMeshes[i]->mPrimitiveTypes != 4) {
			zt_logInfo("moco: skipping non-polygonal mesh (%s - %d)", scene->mMeshes[i]->mName.C_Str(), scene->mMeshes[i]->mPrimitiveTypes);
			meshes_to_skip += 1;
		}
	}


	ztSerial serial;
	if (!zt_serialMakeWriter(&serial, file_out, ZT_MODEL_FILE_IDENTIFIER, ZT_MODEL_FILE_VERSION)) {
		if (error) *error = MocoErrorType_OutputFileWriteError;
		return false;
	}

	// write header
	zt_serialGroupPush(&serial);
	{
		zt_serialWrite(&serial, ZT_MODEL_FILE_GUID);

		const char *writer = "zt_moco model converter";
		zt_serialWrite(&serial, writer, zt_strLen(writer));
		zt_serialWrite(&serial, zt_getDate());
	}
	zt_serialGroupPop(&serial);

	// write bone hierarchy
	zt_serialGroupPush(&serial);
	{
		zt_serialGroupPush(&serial);
		{
			zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_BONES);
			zt_serialWrite(&serial, (i32)bones_count);

			zt_serialGroupPush(&serial);
			{
				for (int sort_idx = 0; sort_idx < bones_count; ++sort_idx) {
					bool found = false;
					zt_fiz(bones_count) {
						if (bones[i].sorted_idx == sort_idx) {
							found = true;
							zt_serialGroupPush(&serial);
							{
								zt_serialWrite(&serial, bones[i].name, zt_strLen(bones[i].name));
								zt_serialWriteMat4(&serial, bones[i].offset, false, from_blender && bones[i].parent_idx >= 0);

								if (bones[i].parent_idx >= 0) {
									i32 sorted_parent_idx = bones[bones[i].parent_idx].sorted_idx;
									zt_serialWrite(&serial, sorted_parent_idx);
								}
								else {
									zt_serialWrite(&serial, (i32)-1);
								}
							}
							zt_serialGroupPop(&serial);
							break;
						}
					}
					zt_assert(found);
				}
			}
			zt_serialGroupPop(&serial);

		}
		zt_serialGroupPop(&serial);
	}
	zt_serialGroupPop(&serial);

	// write meshes
	zt_serialGroupPush(&serial);
	{
		zt_serialGroupPush(&serial);
		{
			zt_serialWrite(&serial, (i32)scene->mNumMeshes);
		}
		zt_serialGroupPop(&serial);

		zt_fiz(scene->mNumMeshes) {
			aiMesh *mesh = scene->mMeshes[i];

			zt_serialGroupPush(&serial);
			{
				zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH);
				zt_serialWrite(&serial, mesh->mName.C_Str(), (i32)mesh->mName.length);
				zt_serialWrite(&serial, (i32)mesh->mMaterialIndex);

				zt_serialWrite(&serial, (i32)mesh->mNumVertices);
				zt_serialWrite(&serial, (i32)(mesh->mTextureCoords[0] != nullptr ? mesh->mNumVertices : 0));
				zt_serialWrite(&serial, (i32)(mesh->mNormals != nullptr ? mesh->mNumVertices : 0));
				zt_serialWrite(&serial, (i32)(mesh->mColors[0] != nullptr ? mesh->mNumVertices : 0));
				zt_serialWrite(&serial, (i32)(mesh->mTangents != nullptr ? mesh->mNumVertices : 0));

				i32 num_indices = 0;
				zt_fxz(mesh->mNumFaces) {
					zt_fyz(mesh->mFaces[x].mNumIndices) {
						num_indices += 1;
					}
				}

				zt_serialWrite(&serial, (i32)num_indices);

				zt_serialWrite(&serial, (i32)mesh->mNumBones);

				zt_serialGroupPush(&serial);
				{
					// vertices
					zt_serialGroupPush(&serial);
					{
						zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_VERTICES);
						zt_fxz(mesh->mNumVertices) {
							if (from_blender) {
								zt_serialWrite(&serial, zt_vec3(mesh->mVertices[x].x * root_bone_scale_multiplier.x, mesh->mVertices[x].z * root_bone_scale_multiplier.z, -mesh->mVertices[x].y * root_bone_scale_multiplier.y));
							}
							else {
								zt_serialWrite(&serial, zt_vec3(mesh->mVertices[x].x * root_bone_scale_multiplier.x, mesh->mVertices[x].y * root_bone_scale_multiplier.y, mesh->mVertices[x].z * root_bone_scale_multiplier.z));
							}
						}
					}
					zt_serialGroupPop(&serial);

					// uvs
					if (mesh->mTextureCoords[0] != nullptr) {
						zt_serialGroupPush(&serial);
						{
							zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_UVS);
							zt_fxz(mesh->mNumVertices) {
								zt_serialWrite(&serial, zt_vec2(mesh->mTextureCoords[0][x].x, mesh->mTextureCoords[0][x].y));
							}
						}
						zt_serialGroupPop(&serial);
					}

					// normals
					if (mesh->mNormals != nullptr) {
						zt_serialGroupPush(&serial);
						{
							zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_NORMALS);
							zt_fxz(mesh->mNumVertices) {
								if (from_blender) {
									ztVec3 normal = zt_vec3(mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z);
									normal = ztQuat::makeFromEuler(-90, 0, 0).rotatePosition(normal);

									zt_serialWrite(&serial, normal);
									//zt_serialWrite(&serial, zt_vec3(mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z));
								}
								else {
									zt_serialWrite(&serial, zt_vec3(mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z));
								}
							}
						}
						zt_serialGroupPop(&serial);
					}

					// colors
					if (mesh->mColors[0] != nullptr) {
						zt_serialGroupPush(&serial);
						{
							zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_COLORS);
							zt_fxz(mesh->mNumVertices) {
								zt_serialWrite(&serial, zt_vec4(mesh->mColors[0][x].r, mesh->mColors[0][x].g, mesh->mColors[0][x].b, mesh->mColors[0][x].a));
							}
						}
						zt_serialGroupPop(&serial);
					}

					// tangents
					if (mesh->mTangents != nullptr) {
						zt_serialGroupPush(&serial);
						{
							zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_TANGENTS);
							zt_fxz(mesh->mNumVertices) {
								if (from_blender) {
									zt_serialWrite(&serial, zt_vec3(mesh->mTangents[x].x, mesh->mTangents[x].z, mesh->mTangents[x].y));
								}
								else {
									zt_serialWrite(&serial, zt_vec3(mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z));
								}
							}
						}
						zt_serialGroupPop(&serial);
					}

					// indices
					if (mesh->mFaces != nullptr) {
						zt_serialGroupPush(&serial);
						{
							zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_INDICES);
							zt_fxz(mesh->mNumFaces) {
								zt_fyz(mesh->mFaces[x].mNumIndices) {
									zt_serialWrite(&serial, (u32)(mesh->mFaces[x].mIndices[y]));
								}
							}
						}
						zt_serialGroupPop(&serial);
					}

					// bones
					if (mesh->mNumBones > 0) {
						zt_serialGroupPush(&serial);
						{
							zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_BONES);

							zt_fxz(mesh->mNumBones) {
								aiBone *bone = mesh->mBones[x];
								zt_serialGroupPush(&serial);
								{
									zt_serialWrite(&serial, bone->mName.C_Str(), (i32)bone->mName.length);
									zt_serialWrite(&serial, (i32)bone->mNumWeights);

									zt_fyz(bone->mNumWeights) {
										zt_serialWrite(&serial, (i32)bone->mWeights[y].mVertexId);
										zt_serialWrite(&serial, bone->mWeights[y].mWeight);
									}
								}
								zt_serialGroupPop(&serial);
							}
						}
						zt_serialGroupPop(&serial);
					}
				}
				zt_serialGroupPop(&serial);
			}
			zt_serialGroupPop(&serial);
		}
	}
	zt_serialGroupPop(&serial);

	// write model hierarchy
	zt_serialGroupPush(&serial);
	{
		struct Node
		{
			static int countMeshesInDescendents(const aiScene *scene, aiNode *node)
			{
				int meshes_to_skip = 0;
				zt_fiz(node->mNumMeshes) {
					if (scene->mMeshes[node->mMeshes[i]]->mPrimitiveTypes != 4) {
						meshes_to_skip += 1;
					}
				}


				int count = node->mNumMeshes - meshes_to_skip;
				zt_fiz(node->mNumChildren) {
					count += countMeshesInDescendents(scene, node->mChildren[i]);
				}
				return count;
			}

			static bool nodeTransformationIsIdentity(aiNode *node)
			{
				return
				   (zt_real32Eq(node->mTransformation.a1, 1) && zt_real32Eq(node->mTransformation.b1, 0) && zt_real32Eq(node->mTransformation.c1, 0) && zt_real32Eq(node->mTransformation.d1, 0) &&
					zt_real32Eq(node->mTransformation.a2, 0) && zt_real32Eq(node->mTransformation.b2, 1) && zt_real32Eq(node->mTransformation.c2, 0) && zt_real32Eq(node->mTransformation.d2, 0) &&
					zt_real32Eq(node->mTransformation.a3, 0) && zt_real32Eq(node->mTransformation.b3, 0) && zt_real32Eq(node->mTransformation.c3, 1) && zt_real32Eq(node->mTransformation.d3, 0) &&
					zt_real32Eq(node->mTransformation.a4, 0) && zt_real32Eq(node->mTransformation.b4, 0) && zt_real32Eq(node->mTransformation.c4, 0) && zt_real32Eq(node->mTransformation.d4, 1));
			}

			static bool serialize(ztSerial *serial, aiNode *node, const aiScene *scene, bool needs_transform, MocoConvertOptions *options)
			{
				i32 children_with_descendents = 0;
				zt_fiz(node->mNumChildren) {
					if (countMeshesInDescendents(scene, node->mChildren[i]) > 0) {
						children_with_descendents += 1;
					}
				}

				if (children_with_descendents == 0 && node->mNumMeshes == 0) {
					return true;
				}

				zt_serialGroupPush(serial);
				{
					zt_serialGroupPush(serial);
					{
						if (needs_transform) {
							//ztMat4 node_offset = zt_mat4(node->mTransformation);
							//ztMat4 final_offset = options->root_transform * node_offset;
							//ztTransform final_offset_transform = zt_transformFromMat4(&final_offset);
							//zt_serialWrite(serial, final_offset_transform.position);
							//zt_serialWrite(serial, final_offset_transform.rotation);// *ztQuat::makeFromEuler(0, 0, 180));
							//zt_serialWrite(serial, final_offset_transform.scale);

							//	return zt_mat4(matrix.a1, matrix.b1, matrix.c1, matrix.d1, matrix.a2, matrix.b2, matrix.c2, matrix.d2, matrix.a3, matrix.b3, matrix.c3, matrix.d3, matrix.a4, matrix.b4, matrix.c4, matrix.d4);

							ztTransform test = zt_transformFromMat4(&options->root_transform);

							//options->root_transform.transpose();

							aiMatrix4x4 root_matrix(options->root_transform.c0r0, options->root_transform.c1r0, options->root_transform.c2r0, options->root_transform.c3r0, 
													options->root_transform.c0r1, options->root_transform.c1r1, options->root_transform.c2r1, options->root_transform.c3r1,
													options->root_transform.c0r2, options->root_transform.c1r2, options->root_transform.c2r2, options->root_transform.c3r2,
													options->root_transform.c0r3, options->root_transform.c1r3, options->root_transform.c2r3, options->root_transform.c3r3);

							aiMatrix4x4 final_matrix = root_matrix;// node->mTransformation;
							zt_serialWriteMat4(serial, final_matrix, false, false);
						}
						else {
							zt_serialWriteMat4(serial, node->mTransformation, false, options && options->from_blender);
						}

						int meshes_to_skip = 0;
						aiMesh *mesh = nullptr;
						zt_fiz(node->mNumMeshes) {
							if (scene->mMeshes[node->mMeshes[i]]->mPrimitiveTypes != 4) {
								meshes_to_skip += 1;
							}
							else {
								mesh = scene->mMeshes[node->mMeshes[i]];
							}
						}

						zt_serialWrite(serial, (i32)(node->mNumMeshes - meshes_to_skip));
						//zt_serialWrite(serial, (i32)node->mNumChildren);
						zt_serialWrite(serial, children_with_descendents);

						if (node->mName.length > 0) {
							zt_serialWrite(serial, node->mName.C_Str(), (i32)node->mName.length);
						}
						else {
							if (node->mNumMeshes == 1) {
								aiMesh *mesh = scene->mMeshes[node->mMeshes[0]];
								zt_serialWrite(serial, mesh->mName.C_Str(), (i32)mesh->mName.length);
							}
							else {
								if (node->mNumMeshes - meshes_to_skip == 1 && mesh != nullptr) {
									zt_serialWrite(serial, mesh->mName.C_Str(), (i32)mesh->mName.length);
								}
								else {
									zt_serialWrite(serial, (char*)nullptr, 0);
								}
							}
						}
					}
					zt_serialGroupPop(serial);

					zt_serialGroupPush(serial);
					{
						zt_fiz(node->mNumMeshes) {
							if (scene->mMeshes[node->mMeshes[i]]->mPrimitiveTypes != 4) {
								continue;
							}

							zt_serialGroupPush(serial);
							{
								zt_serialWrite(serial, (i32)node->mMeshes[i]);
							}
							zt_serialGroupPop(serial);
						}
					}
					zt_serialGroupPop(serial);

					zt_serialGroupPush(serial);
					{
						zt_fiz(node->mNumChildren) {
							if (countMeshesInDescendents(scene, node->mChildren[i]) > 0) {
								serialize(serial, node->mChildren[i], scene, false, nullptr);
							}
						}
					}
					zt_serialGroupPop(serial);
				}
				zt_serialGroupPop(serial);
				return true;
			}
		};

		zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_HIERARCHY);

		int root_children_with_meshes = 0;
		zt_fiz(scene->mRootNode->mNumChildren) {
			if (Node::countMeshesInDescendents(scene, scene->mRootNode->mChildren[i]) > 0) {
				root_children_with_meshes += 1;
			}
		}

		if (root_children_with_meshes > 1 || !Node::nodeTransformationIsIdentity(scene->mRootNode)) {
			Node::serialize(&serial, scene->mRootNode, scene, needs_transform, options);
		}
		else {
			zt_fiz(scene->mRootNode->mNumChildren) {
				if (Node::countMeshesInDescendents(scene, scene->mRootNode->mChildren[i]) > 0) {
					Node::serialize(&serial, scene->mRootNode->mChildren[i], scene, needs_transform, options);
					break;
				}
			}
		}
	}
	zt_serialGroupPop(&serial);

	struct TextureInfo
	{
		char file_name[256];
	};

	TextureInfo textures[64];
	int         textures_idx = 0;

	zt_fiz(scene->mNumTextures) {
		int idx = textures_idx++;
		textures[idx].file_name[0] = 0;
	}

	// write materials
	zt_serialGroupPush(&serial);
	{
		
		zt_serialGroupPush(&serial);
		{
			zt_serialWrite(&serial, (i32)scene->mNumMaterials);
		}
		zt_serialGroupPop(&serial);
		
		zt_fiz(scene->mNumMaterials) {
			struct Material
			{
				static int process(aiMaterial *material, aiTextureType texture_type, const char *file_path, TextureInfo* texture_infos, int texture_infos_size, int *texture_infos_idx)
				{
					int tex_count = material->GetTextureCount(texture_type);

					aiString tex_path;
					material->GetTexture(texture_type, 0, &tex_path);

					const char *path_cstr = tex_path.C_Str();
					if (zt_strStartsWith(path_cstr, "*")) {
						return zt_strToInt(path_cstr + 1, -1);
					}
					else {
						char file_name[ztFileMaxPath];
						zt_fileGetFileInOtherFileDirectory(file_name, ztFileMaxPath, path_cstr, file_path);
						if (zt_fileExists(file_name)) {
							zt_assertReturnValOnFail(*texture_infos_idx < texture_infos_size, -1);
							int tex_idx = (*texture_infos_idx)++;
							zt_strCpy(texture_infos[tex_idx].file_name, zt_elementsOf(texture_infos[tex_idx].file_name), path_cstr);
							return tex_idx;
						}
					}

					return -1;
				}

				static bool serialize(ztSerial *serial, int tex_type, int tex_idx)
				{
					if (tex_idx < 0) {
						return true;
					}

					zt_serialGroupPush(serial);
					{
						zt_serialWrite(serial, (i32)tex_type);
						zt_serialWrite(serial, (i32)tex_idx);
					}
					zt_serialGroupPop(serial);

					return true;
				}
			};

			aiMaterial *material = scene->mMaterials[i];

			i32 tex_diffuse    = Material::process(material, aiTextureType_DIFFUSE,    file_in, textures, zt_elementsOf(textures), &textures_idx);
			i32 tex_normals    = Material::process(material, aiTextureType_NORMALS,    file_in, textures, zt_elementsOf(textures), &textures_idx);
			i32 tex_specular   = Material::process(material, aiTextureType_SPECULAR,   file_in, textures, zt_elementsOf(textures), &textures_idx);
			i32 tex_height     = Material::process(material, aiTextureType_HEIGHT,     file_in, textures, zt_elementsOf(textures), &textures_idx);
			i32 tex_reflection = Material::process(material, aiTextureType_REFLECTION, file_in, textures, zt_elementsOf(textures), &textures_idx);
			i32 tex_emissive   = Material::process(material, aiTextureType_EMISSIVE,   file_in, textures, zt_elementsOf(textures), &textures_idx);

			if (zt_striEndsWith(file_in, ".obj")) {
				// obj uses height (bump) map as normal map, so we need to adjust
				tex_normals = tex_height;
				tex_height = -1;
			}

			aiColor3D diffuse_color;
			material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color);

			aiColor3D specular_color;
			material->Get(AI_MATKEY_COLOR_SPECULAR, specular_color);

			r32 shininess = 0;
			material->Get(AI_MATKEY_SHININESS, shininess);

			zt_serialGroupPush(&serial);
			{
				zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MATERIAL);
				zt_serialWrite(&serial, zt_vec4(diffuse_color.r, diffuse_color.g, diffuse_color.b, 1));
				zt_serialWrite(&serial, zt_vec4(specular_color.r, specular_color.g, specular_color.b, 1));
				zt_serialWrite(&serial, shininess);

				i32 textures_count = (tex_diffuse >= 0 ? 1 : 0) + (tex_normals >= 0 ? 1 : 0) + (tex_specular >= 0 ? 1 : 0) + (tex_height >= 0 ? 1 : 0) + (tex_reflection >= 0 ? 1 : 0) + (tex_emissive >= 0 ? 1 : 0);
				zt_serialWrite(&serial, textures_count);

				zt_serialGroupPush(&serial);
				{
					if (tex_diffuse    >= 0) Material::serialize(&serial, ZT_MODEL_FILE_MATTEXTYPE_DIFFUSE,    tex_diffuse);
					if (tex_normals    >= 0) Material::serialize(&serial, ZT_MODEL_FILE_MATTEXTYPE_NORMALS,    tex_normals);
					if (tex_specular   >= 0) Material::serialize(&serial, ZT_MODEL_FILE_MATTEXTYPE_SPECULAR,   tex_specular);
					if (tex_height     >= 0) Material::serialize(&serial, ZT_MODEL_FILE_MATTEXTYPE_HEIGHT,     tex_height);
					if (tex_reflection >= 0) Material::serialize(&serial, ZT_MODEL_FILE_MATTEXTYPE_REFLECTION, tex_reflection);
					if (tex_emissive   >= 0) Material::serialize(&serial, ZT_MODEL_FILE_MATTEXTYPE_EMISSIVE,   tex_emissive);
				}
				zt_serialGroupPop(&serial);
			}
			zt_serialGroupPop(&serial);
		}
	}
	zt_serialGroupPop(&serial);

	// write textures
	zt_serialGroupPush(&serial);
	{
		zt_serialGroupPush(&serial);
		{
			zt_serialWrite(&serial, textures_idx);
		}
		zt_serialGroupPop(&serial);

		zt_fiz(scene->mNumTextures) {
			zt_serialGroupPush(&serial);
			{
				zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_TEXTURE);

				aiTexture *texture = scene->mTextures[i];
				if (texture->mHeight == 0) { // compressed file format
					zt_serialWrite(&serial, ZT_MODEL_FILE_TEXTURE_IMAGE_FILE);
					zt_serialWrite(&serial, texture->achFormatHint, zt_strLen(texture->achFormatHint));
					zt_serialWrite(&serial, (i32)texture->mWidth);
					zt_serialWrite(&serial, (void*)texture->pcData, texture->mWidth);
				}
				else { // raw pixel data
					zt_serialWrite(&serial, ZT_MODEL_FILE_TEXTURE_IMAGE_PIXELS);
					zt_serialWrite(&serial, (i32)texture->mWidth);
					zt_serialWrite(&serial, (i32)texture->mHeight);
					zt_serialWrite(&serial, texture->pcData, texture->mWidth * texture->mHeight * 4); // in RGB8888 format
				}
			}
			zt_serialGroupPop(&serial);
		}

		for (int i = scene->mNumTextures; i < textures_idx; ++i) {
			zt_serialGroupPush(&serial);
			{
				zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_TEXTURE);
				zt_serialWrite(&serial, ZT_MODEL_FILE_TEXTURE_IMAGE_FILE);

				char file_name[ztFileMaxPath];
				zt_fileGetFileInOtherFileDirectory(file_name, ztFileMaxPath, textures[i].file_name, file_in);

				i32 data_size = 0;
				void *data = zt_readEntireFile(file_name, &data_size);
				if (data) {
					const char *ext = zt_strFindLast(file_name, ".");
					if (ext) ext += 1;

					zt_serialWrite(&serial, ext, zt_strLen(ext));
					zt_serialWrite(&serial, data_size);
					zt_serialWrite(&serial, data, data_size);
					zt_free(data);
				}
				else {
					zt_serialWrite(&serial, "null", 4);
					zt_serialWrite(&serial, 0);
				}
			}
			zt_serialGroupPop(&serial);
		}
	}
	zt_serialGroupPop(&serial);

	// write animations
	zt_serialGroupPush(&serial);
	{
		zt_serialGroupPush(&serial);
		{
			zt_serialWrite(&serial, (i32)scene->mNumAnimations);
		}
		zt_serialGroupPop(&serial);

		zt_fiz(scene->mNumAnimations) {
			aiAnimation *anim = scene->mAnimations[i];
			zt_serialGroupPush(&serial);
			{
				zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_ANIMATION);

				zt_serialWrite(&serial, anim->mName.C_Str(), (i32)anim->mName.length);
				zt_serialWrite(&serial, anim->mDuration);
				zt_serialWrite(&serial, (i32)anim->mTicksPerSecond);
				zt_serialWrite(&serial, (i32)anim->mNumChannels);

				zt_fjz(anim->mNumChannels) {
					zt_serialGroupPush(&serial);
					{
						aiNodeAnim *anim_node = anim->mChannels[j];
						zt_serialWrite(&serial, anim_node->mNodeName.C_Str(), (i32)anim_node->mNodeName.length);

						bool apply_from_blender = false;
						if (from_blender) {
							zt_fxz(bones_count) {
								if (zt_strEquals(bones[x].name, anim_node->mNodeName.C_Str())) {
									if (bones[x].parent_idx >= 0 && bones[bones[x].parent_idx].parent_idx == -1) {
										//apply_from_blender = true;
									}
									break;
								}
							}
						}

						zt_serialWrite(&serial, (i32)anim_node->mNumPositionKeys);
						zt_fkz(anim_node->mNumPositionKeys) {
							zt_serialGroupPush(&serial);
							{
								zt_serialWrite(&serial, anim_node->mPositionKeys[k].mTime);
								if (apply_from_blender) {
									zt_serialWrite(&serial, zt_vec3(anim_node->mPositionKeys[k].mValue.x, anim_node->mPositionKeys[k].mValue.z, -anim_node->mPositionKeys[k].mValue.y));
									//zt_serialWrite(&serial, zt_vec3(anim_node->mPositionKeys[k].mValue.x, anim_node->mPositionKeys[k].mValue.y, anim_node->mPositionKeys[k].mValue.z));
								}
								else {
									zt_serialWrite(&serial, zt_vec3(anim_node->mPositionKeys[k].mValue.x, anim_node->mPositionKeys[k].mValue.y, anim_node->mPositionKeys[k].mValue.z));
								}
							}
							zt_serialGroupPop(&serial);
						}

						zt_serialWrite(&serial, (i32)anim_node->mNumRotationKeys);
						zt_fkz(anim_node->mNumRotationKeys) {
							zt_serialGroupPush(&serial);
							{
								zt_serialWrite(&serial, anim_node->mRotationKeys[k].mTime);
								ztQuat quat = zt_quat(anim_node->mRotationKeys[k].mValue.x, anim_node->mRotationKeys[k].mValue.y, anim_node->mRotationKeys[k].mValue.z, anim_node->mRotationKeys[k].mValue.w);
								if (apply_from_blender) {
									ztVec3 euler = quat.euler();
									quat = ztQuat::makeFromEuler(euler.x - 90, euler.z, euler.y);
									quat.normalize();
									//quat = ztQuat::makeFromEuler(zt_degreesToRadians(euler.x), zt_degreesToRadians(euler.z), zt_degreesToRadians(-euler.y));
									zt_serialWrite(&serial, quat);
								}
								else {
									zt_serialWrite(&serial, quat);
								}
							}
							zt_serialGroupPop(&serial);
						}

						zt_serialWrite(&serial, (i32)anim_node->mNumScalingKeys);
						zt_fkz(anim_node->mNumScalingKeys) {
							zt_serialGroupPush(&serial);
							{
								zt_serialWrite(&serial, anim_node->mScalingKeys[k].mTime);
								if (apply_from_blender) {
									zt_serialWrite(&serial, zt_vec3(anim_node->mScalingKeys[k].mValue.x, anim_node->mScalingKeys[k].mValue.z, anim_node->mScalingKeys[k].mValue.y));
								}
								else {
									zt_serialWrite(&serial, zt_vec3(anim_node->mScalingKeys[k].mValue.x, anim_node->mScalingKeys[k].mValue.y, anim_node->mScalingKeys[k].mValue.z));
								}
							}
							zt_serialGroupPop(&serial);
						}
					}
					zt_serialGroupPop(&serial);
				}

			}
			zt_serialGroupPop(&serial);
		}
	}
	zt_serialGroupPop(&serial);

	zt_serialClose(&serial);

	zt_free(bones);

	aiReleaseImport(scene);
	scene = nullptr;

	return true;
}

// ================================================================================================================================================================================================
