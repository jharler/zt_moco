/**************************************************************************************************
 ** file: src\game_scene_main.cpp
 **
 ** This file was automatically generated.
 **************************************************************************************************/

// headers ========================================================================================================================================================================================

#include "game.h"

// variables ======================================================================================================================================================================================

// private functions ==============================================================================================================================================================================

ztInternal void _gameSceneMainDoLoad(ztGame *game, GameSceneMain *gs, int idx)
{
	switch (idx)
	{
		case GameSceneMainLoad_Scene: {
			if (gs->load.load_info[idx].state == ztLoadState_Query) {
				gs->load.load_info[idx].total_steps  = 2;
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
							ztShaderID shader_pbr = zt_shaderMakePhysicallyBasedRendering(&settings);

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

							// models
							{
								ztMaterial statue_mat = zt_materialMake();
								ztMeshID statue_mesh;

								if (zt_meshLoadOBJ(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/pbr_test_statue.obj"), &statue_mesh, &statue_mat, 1, ztVec3::one * .5f) != 1) {
									gs->load.load_info[idx].state = ztLoadState_Error;
									break;
								}

								statue_mat = zt_materialMake(
								zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/pbr_test_statue_albedo.png"), ztTextureFlags_MipMaps), ztColor_White, ztMaterialFlags_OwnsTexture,
								zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/pbr_test_statue_metallic.png"), ztTextureFlags_MipMaps), ztColor_White, ztMaterialFlags_OwnsTexture,
								zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/pbr_test_statue_normal.png"), ztTextureFlags_MipMaps), ztMaterialFlags_OwnsTexture,
								zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/pbr_test_statue_height.png"), ztTextureFlags_MipMaps), ztMaterialFlags_OwnsTexture,
								zt_textureMake(&game->asset_manager, zt_assetLoad(&game->asset_manager, "models/pbr_test_statue_roughness.png"), ztTextureFlags_MipMaps), ztMaterialFlags_OwnsTexture);

								ztModel *statue_model = &gs->models[gs->models_used++];
								zt_modelMakeFromMesh(statue_model, statue_mesh, &statue_mat, shader_pbr, nullptr, ztModelFlags_CastsShadows);
								statue_model->aabb_center = ztVec3::zero;
								statue_model->aabb_size = zt_vec3(1, .1f, 1);
								statue_model->transform.rotation = ztQuat::makeFromEuler(0, 180, 0);
								zt_sceneAddModel(gs->scene, statue_model);
							}
							{
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

							// lights
							{
								ztLight *light = &gs->lights[gs->lights_used++];
								*light = zt_lightMakeDirectional(zt_vec3(3, 10, 3), ztVec3::zero, 1, 0.25f);
								zt_sceneAddLight(gs->scene, light);

								light = &gs->lights[gs->lights_used++];
								*light = zt_lightMakeArea(zt_vec3(-3, 5, -3), 1.f, true, ztColor_Cyan);
								zt_sceneAddLight(gs->scene, light);
							}
						}
						game->camera_3d.rotation = CAM_START_ROT;
						game->camera_3d.position = CAM_START_POS;
						gs->camera_controller = zt_cameraControllerMakeArcball(&game->camera_3d);
					} break;

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

FUNC_GAME_SCENE_UPDATE(gameSceneMainUpdate)
{
	ZT_PROFILE_GAME("gameSceneMainUpdate");

	GameSceneMain *gs = &game->game_scene_main;

	if (input_keys[ztInputKeys_Control].pressed() && input_keys[ztInputKeys_Shift].pressed() && input_keys[ztInputKeys_Menu].pressed()) {
		// put special debugging stuff here
	}

	if (input_keys[ztInputKeys_Escape].justPressed()) {
		game->camera_3d.rotation = CAM_START_ROT;
		game->camera_3d.position = CAM_START_POS;
		gs->camera_controller = zt_cameraControllerMakeArcball(&game->camera_3d);
	}

	zt_cameraControlUpdateArcball(&gs->camera_controller, input_mouse, input_keys, dt);

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

			zt_renderDrawList(&game->camera_3d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoClear);
		}
	}
	zt_textureRenderTargetCommit(final_render_target);
}

// ================================================================================================================================================================================================
