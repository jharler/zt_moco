/**************************************************************************************************
 ** file: src\game_scene_main.cpp
 **
 ** This file was automatically generated.
 **************************************************************************************************/

// headers ========================================================================================================================================================================================

#include "game.h"
#include "moco.h"
#include "moco_gui.h"

// variables ======================================================================================================================================================================================

// private functions ==============================================================================================================================================================================

ztInternal void _gameSceneMainDoLoad(ztGame *game, GameSceneMain *gs, int idx)
{
	switch (idx)
	{
		case GameSceneMainLoad_Scene: {
			if (gs->load.load_info[idx].state == ztLoadState_Query) {
				gs->load.load_info[idx].total_steps  = 3;
				gs->load.load_info[idx].current_step = 0;
				gs->load.load_info[idx].state        = ztLoadState_Loading;
			}
			else if (gs->load.load_info[idx].state == ztLoadState_Loading) {
				gs->game = game;

				switch (gs->load.load_info[idx].current_step++)
				{
					case 0: {
						gs->scene = zt_sceneMake(zt_memGetGlobalArena());
						{
							ztShaderPhysicallyBasedRenderingSettings settings = {};
							settings.support_bones = true;
							settings.max_bones = 128;
							//ztShaderID shader_pbr = zt_shaderMakePhysicallyBasedRendering(&settings);
							ztShaderID shader_pbr = zt_shaderMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "shaders/shader_pbr.zts"));

							gs->shader_pbr = shader_pbr;
							gs->shader_lit_shadow = zt_shaderGetDefault(ztShaderDefault_LitShadow);

							// environment maps
							{
								ztTextureID tex = zt_textureMakeCubeMapFromHDR(&game->asset_manager, zt_assetLoad(&game->asset_manager, "textures/environment.hdr"), 1024, 1024);
								if (tex) {
									gs->scene->tex_irradiance_map = zt_textureMakeIrradianceCubeMapFromCubeMap(tex);
									gs->scene->tex_prefilter_map = zt_textureMakePrefilterCubeMapFromCubeMap(tex);

									//ztModel *model_skybox = &gs->models[gs->models_used++];
									//if (zt_modelMakeSkybox(model_skybox, tex, true)) {
									//	zt_sceneSetSkybox(gs->scene, model_skybox);
									//}
								}

								gs->scene->tex_brdf_lut = zt_textureMakeBidirectionalReflectanceDistributionFunctionLUT(512, 512);
							}

							if (false) {
								ztMaterial floor_mat = zt_materialMake();
								ztMeshID floor_panel = zt_meshMakePrimitivePlane(3, 3, 3, 3);

								floor_mat = zt_materialMake(
									zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/floor_panel_albedo.png"), ztTextureFlags_MipMaps), ztColor_White, ztMaterialFlags_OwnsTexture,
									zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/floor_panel_metallic.png"), ztTextureFlags_MipMaps), ztColor_White, ztMaterialFlags_OwnsTexture,
									zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/floor_panel_normal.png"), ztTextureFlags_MipMaps), ztMaterialFlags_OwnsTexture,
									zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/floor_panel_height.png"), ztTextureFlags_MipMaps), ztMaterialFlags_OwnsTexture,
									zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/floor_panel_roughness.png"), ztTextureFlags_MipMaps), ztMaterialFlags_OwnsTexture);

								ztModel *floor_model = &gs->models[gs->models_used++];
								zt_modelMakeFromMesh(floor_model, floor_panel, &floor_mat, shader_pbr, nullptr, ztModelFlags_OwnsMaterials | ztModelFlags_OwnsMesh);
								floor_model->aabb_center = ztVec3::zero;
								floor_model->aabb_size = zt_vec3(3, .1f, 3);
								floor_model->transform.position.y = .01f;
								zt_sceneAddModel(gs->scene, floor_model);
							}
						}
						// lights
						{
							ztLight *light = &gs->lights[gs->lights_used++];
							*light = zt_lightMakeDirectional(zt_vec3(30, 100, 30), ztVec3::zero, 1, 0.25f);
							zt_sceneAddLight(gs->scene, light);

							//light = &gs->lights[gs->lights_used++];
							//*light = zt_lightMakeArea(zt_vec3(-3, 5, -3), 1.f, true, ztColor_Cyan);
							//zt_sceneAddLight(gs->scene, light);
						}

						game->camera_3d.rotation = CAM_START_ROT;
						game->camera_3d.position = CAM_START_POS;
						gs->camera_controller = zt_cameraControllerMakeArcball(&game->camera_3d);
					} break;

					case 1: {
						mocoGuiCreateToolbar(game);
					}

					default: {
						gs->load.load_info[idx].state = ztLoadState_Complete;
					} break;
				}
			}
		} break;

		default: zt_assert(false);
	}
}

// functions ======================================================================================================================================================================================

FUNC_GAME_SCENE_BEGIN(gameSceneMainLoadingBegin)
{
	ZT_PROFILE_GAME("gameSceneMainLoadingBegin");

	GameSceneMain *gs = &game->game_scene_main;

	zt_fize(gs->load.load_info) {
		gs->load.load_info[i] = {};
		_gameSceneMainDoLoad(game, gs, i);
	}

	gs->load.load_time = 0;
	gs->load.load_idx  = 0;

	return true;
}

// ================================================================================================================================================================================================

FUNC_GAME_SCENE_CLEANUP(gameSceneMainLoadingCleanup)
{
	GameSceneMain *gs = &game->game_scene_main;
	// cleanup here
}

// ================================================================================================================================================================================================

FUNC_GAME_SCENE_UPDATE(gameSceneMainLoadingUpdate)
{
	ZT_PROFILE_GAME("gameSceneMainLoadingUpdate");

	GameSceneMain *gs = &game->game_scene_main;

	if (game->game_scene_transition_to != GameScene_Invalid || game->fade_time > 0) {
			return true;
	}

	gs->load.load_time += dt;

	if (gs->load.load_idx >= zt_elementsOf(gs->load.load_info)) {
		if (gs->load.load_time > .025f) {
			gameTransitionTo(game, GameScene_Main, false);
		}
		return true;
	}

	_gameSceneMainDoLoad(game, gs, gs->load.load_idx);

	if (gs->load.load_info[gs->load.load_idx].state == ztLoadState_Complete) {
		if (++gs->load.load_idx >= zt_elementsOf(gs->load.load_info)) {
			gs->load.load_time = 0;
		}
	}
	else if (gs->load.load_info[gs->load.load_idx].state == ztLoadState_Error) {
		if (gs->load.load_time > 3) {
			return false;
		}
	}

	return true;
}

// ================================================================================================================================================================================================

FUNC_GAME_SCENE_RENDER(gameSceneMainLoadingRender)
{
	ZT_PROFILE_GAME("gameSceneMainLoadingRender");

	GameSceneMain *gs = &game->game_scene_main;

	zt_drawListPushShader(&game->draw_list, zt_shaderGetDefault(ztShaderDefault_Unlit));
	{
		if (gs->load.load_idx < zt_elementsOf(gs->load.load_info) && gs->load.load_info[gs->load.load_idx].state == ztLoadState_Error) {
			zt_drawListAddFancyText2D(&game->draw_list, ztFontDefault, "Error loading resources", ztVec2::zero, ztAlign_Center, ztAnchor_Center, nullptr, ztColor_Black);
		}
		else {
			zt_drawListAddFancyText2D(&game->draw_list, ztFontDefault, "Loading", ztVec2::zero, ztAlign_Center, ztAnchor_Center, nullptr, ztColor_Black);
			int total = 0, current = 0;

			zt_fize(gs->load.load_info) {
				total += gs->load.load_info[i].total_steps;
				current += gs->load.load_info[i].current_step;
			}

			r32 pct = (r32)current / total;
			r32 pix = 1 / zt_pixelsPerUnit();

			ztVec2 load_ttl = zt_vec2(6, .25f);
			ztVec2 load_pct = zt_vec2(6 * pct, .25f);

			r32 diff = load_ttl.x - load_pct.x;
			load_ttl.x += pix * 8;
			load_ttl.y += pix * 8;

			zt_drawListPushTexture(&game->draw_list, 0);
			{
				zt_drawListAddSolidRect2D(&game->draw_list, zt_vec3(0, -.5f, 0), load_ttl, ztColor_Black);
				zt_drawListAddSolidRect2D(&game->draw_list, zt_vec3(-diff / 2, -.5f, 0), load_pct, ztColor_White);
			}
			zt_drawListPopTexture(&game->draw_list);
		}
	}
	zt_drawListPopShader(&game->draw_list);

	zt_renderDrawList(&game->camera_2d, &game->draw_list, ztColor_White, ztRenderDrawListFlags_NoDepthTest, final_render_target);
}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

FUNC_GAME_SCENE_BEGIN(gameSceneMainBegin)
{
	GameSceneMain *gs = &game->game_scene_main;

	//zt_profilerPause(); // uncomment to profile startup

	return true;
}

// ================================================================================================================================================================================================

FUNC_GAME_SCENE_CLEANUP(gameSceneMainCleanup)
{
	GameSceneMain *gs = &game->game_scene_main;

	zt_sceneFree(gs->scene);

	zt_fiz(gs->models_used) {
		zt_modelFree(&gs->models[i]);
	}
	gs->models_used = 0;

	gs->scene = nullptr;
}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ztInternal ztModel *_gsm_modelUnderCursor(ztGame *game, GameSceneMain *gs)
{
	if (gs->root_model == nullptr) {
		return nullptr;
	}

	struct Models
	{
		static void findAllCollisions(ztModel *model, ztVec3 ray_pos, ztVec3 ray_dir, ztModel **collisions, int collisions_size, int *collisions_idx)
		{
			ztVec3 aabb_center, aabb_size;
			zt_modelGetAABB(model, &aabb_center, &aabb_size);

			zt_bitRemove(model->flags, ztModelFlags_DebugDrawAABB);

			if (zt_collisionRayInAABB(ray_pos, ray_dir, aabb_center, aabb_size)) {
				if (*collisions_idx < collisions_size) {
					collisions[(*collisions_idx)++] = model;
				}
			}

			zt_flink(child, model->first_child) {
				findAllCollisions(child, ray_pos, ray_dir, collisions, collisions_size, collisions_idx);
			}
		}
	};

	ztModel *collisions[64];
	int collisions_count = 0;

	Models::findAllCollisions(gs->root_model, gs->mouse_ray_pos, gs->mouse_ray_dir, collisions, zt_elementsOf(collisions), &collisions_count);

	zt_fiz(collisions_count) {
		collisions[i]->flags |= ztModelFlags_DebugDrawAABB;
	}

	return collisions_count > 0 ? collisions[0] : nullptr;

}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================


FUNC_GAME_SCENE_UPDATE(gameSceneMainUpdate)
{
	ZT_PROFILE_GAME("gameSceneMainUpdate");

	GameSceneMain *gs = &game->game_scene_main;

	if (input_keys[ztInputKeys_Control].pressed() && input_keys[ztInputKeys_Shift].pressed() && input_keys[ztInputKeys_Menu].pressed()) {
		// put special debugging stuff here
	}

	zt_cameraPerspGetMouseRay(&game->camera_3d, input_mouse->screen_x, input_mouse->screen_y, &gs->mouse_ray_pos, &gs->mouse_ray_dir);
	//_gsm_modelUnderCursor(game, gs);


	if (!gui_input) {
		if (input_keys[ztInputKeys_Escape].justPressed()) {
			game->camera_3d.rotation = CAM_START_ROT;
			game->camera_3d.position = CAM_START_POS;
			gs->camera_controller = zt_cameraControllerMakeArcball(&game->camera_3d);
		}

		if (input_keys[ztInputKeys_T].justPressed()) {
			zt_modelEditWidgetChangeMode(&gs->model_edit_widget, ztModelEditWidgetMode_Translate);
		}
		if (input_keys[ztInputKeys_R].justPressed()) {
			zt_modelEditWidgetChangeMode(&gs->model_edit_widget, ztModelEditWidgetMode_Rotate);
		}
		if (input_keys[ztInputKeys_S].justPressed()) {
			zt_modelEditWidgetChangeMode(&gs->model_edit_widget, ztModelEditWidgetMode_Scale);
		}

		zt_modelEditWidgetUpdate(&gs->model_edit_widget, input_keys, input_mouse, &game->camera_3d, dt);

		zt_cameraControlUpdateArcball(&gs->camera_controller, input_mouse, input_keys, dt, ztCameraControllaerArcballUpdateFlags_IgnoreKeys);
	}

	return true;
}

// ================================================================================================================================================================================================

FUNC_GAME_SCENE_RENDER(gameSceneMainRender)
{
	ZT_PROFILE_GAME("gameSceneMainRender");

	GameSceneMain *gs = &game->game_scene_main;

	zt_scenePrepare(gs->scene, &game->camera_3d);
	zt_sceneOptimize(gs->scene, &game->camera_3d);
	zt_sceneLighting(gs->scene, &game->camera_3d);

	zt_textureRenderTargetPrepare(final_render_target);
	{
		zt_rendererClear(zt_vec4(.024f, .024f, .024f, 1));
		zt_rendererSetDepthTest(true, ztRendererDepthTestFunction_LessEqual);

		zt_sceneRender(gs->scene, &game->camera_3d);

		{
			zt_drawListPushShader(&game->draw_list, zt_shaderGetDefault(ztShaderDefault_Unlit));
			zt_drawListPushTexture(&game->draw_list, ztTextureDefault);
			zt_drawListAddAxis(&game->draw_list, .0075f, game->camera_3d.position + game->camera_3d.direction * .125f);
			zt_drawListAddFloorGrid(&game->draw_list, ztVec3::zero, 10, 10);
			zt_drawListPopTexture(&game->draw_list);
			zt_drawListPopShader(&game->draw_list);

			struct Models
			{
				static void renderBones(ztDrawList *draw_list, ztModel *model, ztBone *bone)
				{
					zt_drawListPushTransform(draw_list, model->calculated_mat * bone->mat_offset * bone->mat_model);
					zt_drawListPushColor(draw_list, zt_bitIsSet(bone->flags, ztBoneFlags_DebugDrawHighlight) ? ztColor_Red : ztColor_Cyan);
					zt_drawListAddEmptySimpleSphere(draw_list, ztVec3::zero, .125f, 16);

					ztVec3 end_pos;

					if (bone->first_child != nullptr && bone->first_child->next == nullptr) {
						end_pos = bone->first_child->transform.position * 1.f;
					}
					else {
						end_pos = zt_vec3(0, .5f, 0);
					}

					zt_drawListAddLine(draw_list, ztVec3::zero, end_pos);
					zt_drawListPopColor(draw_list);

					zt_drawListPushColor(draw_list, zt_bitIsSet(bone->flags, ztBoneFlags_DebugDrawHighlight) ? ztColor_Purple : ztColor_Green);
					zt_drawListAddEmptySimpleSphere(draw_list, end_pos, .05f, 8);
					zt_drawListPopColor(draw_list);
					zt_drawListPopTransform(draw_list);

//					zt_drawListPushColor(draw_list, zt_bitIsSet(bone->flags, ztBoneFlags_DebugDrawHighlight) ? ztColor_Purple : ztColor_Green);
//					zt_drawListAddEmptyCubeFromCenterSize(draw_list, model->calculated_mat.getMultiply(bone->mat_model.getInverse().getMultiply(ztVec3::zero)), zt_vec3(.25f, .25f, .25f));
//					zt_drawListPopColor(draw_list);

					zt_flink(child, bone->first_child) {
						renderBones(draw_list, model, child);
					}
				}

				static void renderBones(ztDrawList *draw_list, ztModel *model)
				{
					if (model->bones_count) {
						renderBones(draw_list, model, &model->bones[model->bones_root_idx]);
					}

					zt_flink(child, model->first_child) {
						renderBones(draw_list, child);
					}
				}

				static void renderBoundingBoxes(ztDrawList *draw_list, ztModel *model)
				{
					//if (zt_bitIsSet(model->flags, ztModelFlags_DebugDrawAABB)) {
					ztColor color = zt_bitIsSet(model->flags, ztModelFlags_DebugDrawAABB) ? ztColor_Red : ztColor_White;

					ztVec3 aabb_pos, aabb_size;
					zt_modelGetAABB(model, &aabb_pos, &aabb_size);

					ztTransform transform = zt_transformFromMat4(&model->calculated_mat);

					zt_drawListPushColor(draw_list, color);
					zt_drawListAddEmptyCubeFromCenterSize(draw_list, aabb_pos, aabb_size);
					zt_drawListPopColor(draw_list);
					//}

					zt_flink(child, model->first_child) {
						renderBoundingBoxes(draw_list, child);
					}
				}
			};

			if (gs->root_model) {
				Models::renderBoundingBoxes(&game->draw_list, gs->root_model);
			}

			zt_renderDrawList(&game->camera_3d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoClear);

			if (gs->root_model) {
				zt_rendererClearDepth();
				zt_drawListPushShader(&game->draw_list, zt_shaderGetDefault(ztShaderDefault_Unlit));
				zt_drawListPushTexture(&game->draw_list, ztTextureDefault);

				if (gs->root_model) {
					Models::renderBones(&game->draw_list, gs->root_model);
				}

				zt_modelEditWidgetRender(&gs->model_edit_widget, &game->camera_3d, &game->draw_list);

				zt_drawListPopTexture(&game->draw_list);
				zt_drawListPopShader(&game->draw_list);

				zt_renderDrawList(&game->camera_3d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoClear);

				zt_drawListPushShader(&game->draw_list, zt_shaderGetDefault(ztShaderDefault_Unlit));
				zt_drawListPushTexture(&game->draw_list, ztTextureDefault);

				zt_modelEditWidgetRenderText(&gs->model_edit_widget, &game->camera_3d, &game->camera_2d, ztFontDefault, zt_vec2(0, -1.5f), &game->draw_list);

				zt_drawListPopTexture(&game->draw_list);
				zt_drawListPopShader(&game->draw_list);

				zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoClear);
			}
		}
	}
	zt_textureRenderTargetCommit(final_render_target);
}

// ================================================================================================================================================================================================
