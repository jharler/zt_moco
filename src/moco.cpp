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
	"Assimp ziImportFile function failed",
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
	//return zt_mat4(matrix.a1, matrix.a2, matrix.a3, matrix.a4, matrix.b1, matrix.b2, matrix.b3, matrix.b4, matrix.c1, matrix.c2, matrix.c3, matrix.c4, matrix.d1, matrix.d2, matrix.d3, matrix.d4);
	return zt_mat4(matrix.a1, matrix.b1, matrix.c1, matrix.d1, matrix.a2, matrix.b2, matrix.c2, matrix.d2, matrix.a3, matrix.b3, matrix.c3, matrix.d3, matrix.a4, matrix.b4, matrix.c4, matrix.d4);
}

// ================================================================================================================================================================================================

bool zt_serialWrite(ztSerial *serial, aiMatrix4x4 matrix)
{
	aiVector3D position;
	aiQuaternion rotation;
	aiVector3D scaling;

	matrix.Decompose(scaling, rotation, position);

//	ztQuat quat = ztQuat::makeFromMat4(zt_mat4(matrix.a1, matrix.a2, matrix.a3, matrix.a4, matrix.b1, matrix.b2, matrix.b3, matrix.b4, matrix.c1, matrix.c2, matrix.c3, matrix.c4, matrix.d1, matrix.d2, matrix.d3, matrix.d4));
//	ztQuat quat = ztQuat::makeFromMat4(zt_mat4(matrix.a1, matrix.b1, matrix.c1, matrix.d1, matrix.a2, matrix.b2, matrix.c2, matrix.d2, matrix.a3, matrix.b3, matrix.c3, matrix.d3, matrix.a4, matrix.b4, matrix.c4, matrix.d4));

	if (!zt_serialWrite(serial, zt_vec3(position.x, position.y, position.z)) ||
		!zt_serialWrite(serial, zt_quat(rotation.x, rotation.y, rotation.z, rotation.w)) ||
		!zt_serialWrite(serial, zt_vec3(scaling.x, scaling.y, scaling.z))) {
		return false;
	}

//	return zt_serialWrite(serial, zt_mat4(matrix));

	return true;
}

// functions ======================================================================================================================================================================================

int mocoGetErrorMessage(MocoErrorType_Enum error_type, char *buffer, int buffer_size)
{
	return zt_strCpy(buffer, buffer_size, _mocoErrorStrings[error_type]);
}

// ================================================================================================================================================================================================

bool mocoConvertFile(const char *file_in, const char *file_out, MocoErrorType_Enum *error)
{
	if (!zt_fileExists(file_in)) {
		if (error) *error = MocoErrorType_InputFileNotFound;
		return false;
	}

	aiLogStream log_stream;
	log_stream.callback = _aiLogStreamCallback;
	log_stream.user = nullptr;

	aiAttachLogStream(&log_stream);
	aiEnableVerboseLogging(true);

	const aiScene *scene = aiImportFile(file_in, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_FindDegenerates | aiProcess_FlipUVs);

	aiDetachLogStream(&log_stream);

	if (scene == nullptr) {
		if (error) *error = MocoErrorType_aiImportFileFailed;
		return false;
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
				zt_serialWrite(&serial, (i32)(mesh->mTextureCoords != nullptr ? mesh->mNumVertices : 0));
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

				i32 num_bones = mesh->mNumBones > 0 ? mesh->mNumBones + 1 : 0;
				zt_serialWrite(&serial, num_bones);

				zt_serialGroupPush(&serial);
				{
					// vertices
					zt_serialGroupPush(&serial);
					{
						zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_VERTICES);
						zt_fxz(mesh->mNumVertices) {
							zt_serialWrite(&serial, zt_vec3(mesh->mVertices[x].x, mesh->mVertices[x].y, mesh->mVertices[x].z));
						}
					}
					zt_serialGroupPop(&serial);

					// uvs
					if (mesh->mTextureCoords != nullptr) {
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
								zt_serialWrite(&serial, zt_vec3(mesh->mNormals[x].x, mesh->mNormals[x].y, mesh->mNormals[x].z));
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
								zt_serialWrite(&serial, zt_vec3(mesh->mTangents[x].x, mesh->mTangents[x].y, mesh->mTangents[x].z));
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
						struct Bone
						{
							static i32 findParent(const aiScene *scene, aiMesh *mesh, aiNode *root, const char *bone_name, aiMatrix4x4 *parent_transform)
							{
								zt_fiz(root->mNumChildren) {
									if (zt_strEquals(root->mChildren[i]->mName.C_Str(), bone_name)) {
										// find the bone index that matches the root name
										zt_fjz(mesh->mNumBones) {
											if (zt_strEquals(mesh->mBones[j]->mName.C_Str(), root->mName.C_Str())) {
												*parent_transform = root->mChildren[i]->mTransformation;
												return j;
											}
										}
									}
								}

								// didn't find a matching child name in this node, check further down the hierarchy
								zt_fiz(root->mNumChildren) {
									i32 parent = findParent(scene, mesh, root->mChildren[i], bone_name, parent_transform);
									if (parent != -1) {
										return parent;
									}
								}

								return -1; // didn't find any matching bones
							}
						};

						zt_serialGroupPush(&serial);
						{
							zt_serialWrite(&serial, ZT_MODEL_FILE_GUID_MESH_BONES);

							zt_serialGroupPush(&serial);
							{
								const char *name = "Armature";
								zt_serialWrite(&serial, name, (i32)zt_strLen(name));
								zt_serialWrite(&serial, (i32)-1);
								zt_serialWrite(&serial, ztVec3::zero);
								zt_serialWrite(&serial, ztQuat::identity);
								zt_serialWrite(&serial, ztVec3::one);
								zt_serialWrite(&serial, ztVec3::zero);
								zt_serialWrite(&serial, ztQuat::identity);
								zt_serialWrite(&serial, ztVec3::one);
								zt_serialWrite(&serial, (i32)0);
							}
							zt_serialGroupPop(&serial);

							zt_fxz(mesh->mNumBones) {
								aiBone *bone = mesh->mBones[x];
								aiMatrix4x4 parent_matrix = scene->mRootNode->mTransformation;
								i32 bone_parent_idx = Bone::findParent(scene, mesh, scene->mRootNode, bone->mName.C_Str(), &parent_matrix);

								zt_serialGroupPush(&serial);
								{
									zt_serialWrite(&serial, bone->mName.C_Str(), (i32)bone->mName.length);
									zt_serialWrite(&serial, bone_parent_idx + 1);
									zt_serialWrite(&serial, parent_matrix);
									zt_serialWrite(&serial, bone->mOffsetMatrix);
									zt_serialWrite(&serial, (i32)bone->mNumWeights);

									ztMat4 my_mat = zt_mat4(bone->mOffsetMatrix);
									ztMat4 my_mat_inv = my_mat.getInverse();
									aiMatrix4x4 ai_mat_inv = bone->mOffsetMatrix;
									ai_mat_inv.Inverse();

									/*
									res.x = (pMatrix.a1 * pVector.x) + (pMatrix.a2 * pVector.y) + (pMatrix.a3 * pVector.z) + pMatrix.a4;
									res.y = (pMatrix.b1 * pVector.x) + (pMatrix.b2 * pVector.y) + (pMatrix.b3 * pVector.z) + pMatrix.b4;
									res.z = (pMatrix.c1 * pVector.x) + (pMatrix.c2 * pVector.y) + (pMatrix.c3 * pVector.z) + pMatrix.c4;
									*/
									aiVector3D bone_offset = bone->mOffsetMatrix * aiVector3D(0, 0, 0);
									aiVector3D bone_position = ai_mat_inv * aiVector3D(0, 0, 0);

									ztVec3 my_bone_offset = my_mat.getMultiply(ztVec3::zero);
									ztVec3 my_bone_position = my_mat_inv.getMultiply(ztVec3::zero);

									ztTransform my_bone_offset_tran = zt_transformFromMat4(&my_mat);
									ztTransform m_bone_position_tran = zt_transformFromMat4(&my_mat_inv);

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
			static int countMeshesInDescendents(aiNode *node)
			{
				int count = node->mNumMeshes;
				zt_fiz(node->mNumChildren) {
					count += countMeshesInDescendents(node->mChildren[i]);
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

			static bool serialize(ztSerial *serial, aiNode *node, const aiScene *scene)
			{
				i32 children_with_descendents = 0;
				zt_fiz(node->mNumChildren) {
					if (countMeshesInDescendents(node->mChildren[i]) > 0) {
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
						zt_serialWrite(serial, node->mTransformation);
						zt_serialWrite(serial, (i32)node->mNumMeshes);
						//zt_serialWrite(serial, (i32)node->mNumChildren);
						zt_serialWrite(serial, children_with_descendents);
						zt_serialWrite(serial, node->mName.C_Str(), (i32)node->mName.length);
					}
					zt_serialGroupPop(serial);

					zt_serialGroupPush(serial);
					{
						zt_fiz(node->mNumMeshes) {
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
							if (countMeshesInDescendents(node->mChildren[i]) > 0) {
								serialize(serial, node->mChildren[i], scene);
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
			if (Node::countMeshesInDescendents(scene->mRootNode->mChildren[i]) > 0) {
				root_children_with_meshes += 1;
			}
		}

		if (root_children_with_meshes > 1 || !Node::nodeTransformationIsIdentity(scene->mRootNode)) {
			Node::serialize(&serial, scene->mRootNode, scene);
		}
		else {
			zt_fiz(scene->mRootNode->mNumChildren) {
				if (Node::countMeshesInDescendents(scene->mRootNode->mChildren[i]) > 0) {
					Node::serialize(&serial, scene->mRootNode->mChildren[i], scene);
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
			zt_serialWrite(&serial, textures_idx);
		}
		zt_serialGroupPop(&serial);
	}
	zt_serialGroupPop(&serial);


	zt_serialClose(&serial);

	aiReleaseImport(scene);
	scene = nullptr;

	return true;
}

// ================================================================================================================================================================================================
