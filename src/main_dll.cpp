/**************************************************************************************************
 ** file: src\main_dll.cpp
 **
 ** This file was automatically generated.
 **************************************************************************************************/

// headers ========================================================================================================================================================================================

#include "game.h"

// variables ======================================================================================================================================================================================

// private functions ==============================================================================================================================================================================

ztInternal void _gameSetFunctionPointers(ztGame *game)
{
	zt_fiz(GameScene_MAX) {
		game->gsf_begin  [i] = nullptr;
		game->gsf_cleanup[i] = nullptr;
		game->gsf_update [i] = nullptr;
		game->gsf_render [i] = nullptr;
	}

	game->gsf_begin  [GameScene_MainLoading  ] = gameSceneMainLoadingBegin;
	game->gsf_cleanup[GameScene_MainLoading  ] = gameSceneMainLoadingCleanup;
	game->gsf_update [GameScene_MainLoading  ] = gameSceneMainLoadingUpdate;
	game->gsf_render [GameScene_MainLoading  ] = gameSceneMainLoadingRender;

	game->gsf_begin  [GameScene_Main         ] = gameSceneMainBegin;
	game->gsf_cleanup[GameScene_Main         ] = gameSceneMainCleanup;
	game->gsf_update [GameScene_Main         ] = gameSceneMainUpdate;
	game->gsf_render [GameScene_Main         ] = gameSceneMainRender;
}

// ================================================================================================================================================================================================

ztInternal void _gameSceneCleanup(ztGame *game)
{
	if (game->gsf_cleanup[game->game_scene]) {
		game->gsf_cleanup[game->game_scene](game, game->game_scene_transition_to);
	}
}

// functions ======================================================================================================================================================================================

ZT_DLLEXPORT bool dll_settings(ztGameDetails* details, ztGameSettings* settings)
{
	settings->memory   = zt_megabytes(256);
#if 0
	settings->native_w = settings->screen_w = 1920;
	settings->native_h = settings->screen_h = 1080;
#else
	settings->native_w = settings->screen_w = 2880;
	settings->native_h = settings->screen_h = 1620;
#endif

	settings->threaded_frame_jobs      = 4;
	settings->threaded_background_jobs = 2;

	if (!zt_cmdHasArg(details->argv, details->argc, "no-vsync", "no-vsync") && !zt_cmdHasArg(details->argv, details->argc, "novsync", "novsync")) {
		settings->renderer_flags |= ztRendererFlags_Vsync;
	}
	return true;
}

// ================================================================================================================================================================================================

ztInternal void _gameCreateRenderTargets(ztGame *game, bool free_first)
{
	if (free_first) {
		if (zt_textureGetSize(game->render_target) == zt_vec2i(game->camera_2d.width, game->camera_2d.height)) {
			return;
		}

		zt_textureFree(game->render_target_attach_position);
		zt_textureFree(game->render_target_attach_normal);
		zt_textureFree(game->render_target_bright);
		zt_textureFree(game->render_target_blurred);
		zt_textureFree(game->render_target_ao);
		zt_textureFree(game->render_target);
	}

	game->render_target                 = zt_textureMakeRenderTarget(game->settings->native_w, game->settings->native_h, ztTextureFlags_HDR | ztTextureFlags_Multisample);
	game->render_target_attach_position = zt_textureRenderTargetAddAttachment(game->render_target, ztTextureColorFormat_RGBA16F);
	game->render_target_attach_normal   = zt_textureRenderTargetAddAttachment(game->render_target, ztTextureColorFormat_RGBA16F);

	game->render_target_bright          = zt_textureMakeRenderTarget(game->settings->native_w / 2, game->settings->native_h / 2, ztTextureFlags_HDR);
	game->render_target_blurred         = zt_textureMakeRenderTarget(game->settings->native_w / 2, game->settings->native_h / 2, ztTextureFlags_HDR);
	game->render_target_ao              = zt_textureMakeRenderTarget(game->settings->native_w / 2, game->settings->native_h / 2);
}

// ================================================================================================================================================================================================

ZT_DLLEXPORT bool dll_init(ztGameDetails* details, ztGameSettings* settings, void** game_memory)
{
	ztGame *game = zt_mallocStruct(ztGame);
	*game = {};
	*game_memory = game;

	game->details     = details;
	game->settings    = settings;


	{
		// app path and assets

		char data_path[ztFileMaxPath];
		zt_strCpy(data_path, ztFileMaxPath, details->data_path);
		if (!zt_directoryExists(data_path)) {
			zt_fileConcatFileToPath(data_path, ztFileMaxPath, details->data_path, "data");
			if (!zt_directoryExists(data_path)) {
				zt_fileConcatFileToPath(data_path, ztFileMaxPath, details->data_path, "run" ztFilePathSeparatorStr "data");
			}
		}
		if (!zt_assetManagerLoadDirectory(&game->asset_manager, data_path, zt_memGetGlobalArena())) {
			zt_logCritical("Unable to load game assets");
			return false;
		}
	}

	{
		// gui

		if (!guiThemeMake(&game->gui_theme, &game->asset_manager, game)) {
			return false;
		}

		game->gui_manager = zt_guiManagerMake(&game->camera_2d, &game->gui_theme, zt_memGetGlobalArena());
		zt_guiInitDebug(game->gui_manager);
	}

	zt_debuggingInit("debug.cfg");

	{
		if (!zt_drawListMake(&game->draw_list, 1024 * 128)) {
			zt_logCritical("Unable to initialize draw list");
			return false;
		}

		_gameCreateRenderTargets(game, false);

		ztRandom random;
		zt_randomInit(&random);
		game->texture_random = zt_textureMakeRandom(&random, 4, 4);

		ztShaderTonemapSettings tonemap_settings = {};
		tonemap_settings.bloom_enabled = false;
		tonemap_settings.ao_enabled = false;

		game->shader_hdr_tonemap = zt_shaderBuildTonemap(&tonemap_settings);
		game->shader_hdr_bright = zt_shaderGetDefault(ztShaderDefault_Bright);
		game->shader_hdr_bloom_blur_1 = zt_shaderGetDefault(ztShaderDefault_BlurHorz);
		game->shader_hdr_bloom_blur_2 = zt_shaderGetDefault(ztShaderDefault_BlurVert);

		ztShaderAmbientOcclusionSettings ao_settings = {};
		ao_settings.generate_noise_tex = false;

		game->shader_ao = zt_shaderBuildAmbientOcclusion(&ao_settings);

		if (game->shader_hdr_tonemap == ztInvalidID || game->shader_hdr_bright == ztInvalidID || game->shader_hdr_bloom_blur_1 == ztInvalidID || game->shader_hdr_bloom_blur_2 == ztInvalidID || game->shader_ao == ztInvalidID) {
			return false;
		}
		zt_shaderSetVariableVec2(game->shader_ao, zt_strHash("noise_scale"), zt_vec2(settings->native_w / 4.0f, settings->native_h / 4.0f));
	}


	_gameSetFunctionPointers(game);

	game->game_scene                 = GameScene_Invalid;
	game->game_scene_transition_to   = GameScene_MainLoading;
	game->game_scene_transition_time = 0;

	game->fade_time = game->fade_time_max = 0;

	zt_tweenManagerMake(&game->tween_manager_state);
	zt_tweenManagerMake(&game->tween_manager_permanent);

#	if defined(ZT_INPUT_REPLAY)
	game->replay_state = ReplayState_None;
	{
		// setup input recording/playback

		if (zt_cmdHasArg(details->argv, details->argc, "i", "input")) {
			char replay_file_name[ztFileMaxPath];
			if (zt_cmdGetArg(details->argv, details->argc, "i", "input", replay_file_name, ztFileMaxPath)) {
				char replay_file[ztFileMaxPath];
				if (zt_fileExists(replay_file_name)) {
					zt_strCpy(replay_file, zt_elementsOf(replay_file), replay_file_name, zt_strLen(replay_file_name));
				}
				else {
					zt_fileConcatFileToPath(replay_file, ztFileMaxPath, details->user_path, replay_file_name);
					if (!zt_fileExists(replay_file)) {
						zt_fileConcatFileToPath(replay_file, ztFileMaxPath, details->app_path, replay_file_name);
					}
				}

				if (!zt_fileExists(replay_file)) {
					zt_logCritical("Could not locate input replay file: %s", replay_file_name);
				}
				else {
					zt_inputReplayMakeReader(&game->replay_data, replay_file);
					game->replaying = true;
					game->replay_state = ReplayState_Replaying;

					zt_guiDebugShowDetails();
				}
			}
		}
		else if (!zt_cmdHasArg(details->argv, details->argc, "no-vsync", "no-vsync")) {
			char replay_file[ztFileMaxPath];

			// we will keep the last 5 replays, so that we don't accidentally lose a replay
			for (int i = 5; i > 1; --i) {
				zt_strMakePrintf(file_name, 64, "input_replay.%d.dat", i);
				zt_fileConcatFileToPath(replay_file, ztFileMaxPath, details->user_path, file_name);

				zt_strPrintf(file_name, 64, "input_replay.%d.dat", i - 1);
				char next_replay_file[ztFileMaxPath];
				zt_fileConcatFileToPath(next_replay_file, ztFileMaxPath, details->user_path, file_name);

				if (zt_fileExists(replay_file)) {
					zt_fileDelete(replay_file);
				}
				if (zt_fileExists(next_replay_file)) {
					zt_fileCopy(next_replay_file, replay_file);
				}
			}

			zt_fileConcatFileToPath(replay_file, ztFileMaxPath, details->user_path, "input_replay.1.dat");
			zt_fileDelete(replay_file);

			zt_inputReplayMakeWriter(&game->replay_data, replay_file);
			game->replaying = false;
			game->replay_state = ReplayState_Recording;
		}
	}
#	endif

	return true;
}

// ================================================================================================================================================================================================

ZT_DLLEXPORT void dll_cleanup(void *memory)
{
	ztGame *game = (ztGame*)memory;

	_gameSceneCleanup(game);
#	if defined(ZT_INPUT_REPLAY)
	zt_inputReplayFree(&game->replay_data);
#	endif
	zt_textureFree(game->render_target);

	zt_textureFree(game->render_target_attach_position);
	zt_textureFree(game->render_target_attach_normal);
	zt_textureFree(game->render_target_bright);
	zt_textureFree(game->render_target_blurred);
	zt_textureFree(game->render_target_ao);

	zt_textureFree(game->render_target);

	zt_textureFree(game->texture_random);
	zt_shaderFree(game->shader_hdr_tonemap);
	zt_shaderFree(game->shader_hdr_bright);
	zt_shaderFree(game->shader_hdr_bloom_blur_1);
	zt_shaderFree(game->shader_hdr_bloom_blur_2);
	zt_shaderFree(game->shader_ao);

	zt_drawListFree(&game->draw_list);
	zt_assetManagerFree(&game->asset_manager);
	guiThemeFree(&game->gui_theme);
	zt_guiManagerFree(game->gui_manager);

	zt_free(game);
}

// ================================================================================================================================================================================================

ZT_DLLEXPORT bool dll_reload(void *memory)
{
	ztGame *game = (ztGame*)memory;
#	if defined(ZT_DLL)
	zt_dllGuiLoad();
#	endif
	_gameSetFunctionPointers(game);
	return true;
}

// ================================================================================================================================================================================================

ZT_DLLEXPORT bool dll_unload(void *memory)
{
#	if defined(ZT_DLL)
	ztGame *game = (ztGame*)memory;
	zt_dllGuiUnload();
#	endif
	return true;
}

// ================================================================================================================================================================================================

void gameAdjustCamera(ztGame *game)
{
	r32 view_distance = 150;
	zt_cameraMakePersp(&game->camera_3d, game->settings->screen_w, game->settings->screen_h, zt_degreesToRadians(60), 0.1f, view_distance, game->camera_3d.position, game->camera_3d.rotation);
	zt_cameraRecalcMatrices(&game->camera_3d);
	_gameCreateRenderTargets(game, true);
}

// ================================================================================================================================================================================================

ZT_DLLEXPORT void dll_screenChange(ztGameSettings *settings, void *memory)
{
	ztGame *game = (ztGame*)memory;
	zt_cameraMakeOrtho(&game->camera_2d, settings->screen_w, settings->screen_h, settings->native_w, settings->native_h, 0.1f, 100.f, game->camera_2d.position);
	zt_cameraRecalcMatrices(&game->camera_2d);
	gameAdjustCamera(game);

	game->screen_updated = true;
}

// ================================================================================================================================================================================================

void gameTransitionTo(ztGame *game, GameScene_Enum state, bool immediately)
{
	game->game_scene_transition_to = state;

	if (immediately) {
		game->game_scene_transition_time = 0;
	}
	else {
		r32 transition_time = .25f;
		game->game_scene_transition_time = transition_time;
		game->fade_time = game->fade_time_max = transition_time * 2;
	}
}

// ================================================================================================================================================================================================

ZT_DLLEXPORT bool dll_gameLoop(void *memory, r32 dt)
{
	ztGame *game = (ztGame*)memory;

	ztInputKeys       input_keys[ztInputKeys_MAX];
	ztInputMouse      input_mouse;
	ztInputController input_controller;
	ztInputKeys_Enum  input_keystrokes[16];
	bool              input_this_frame;
	zt_inputGetKeyStrokes(input_keystrokes);

	bool gui_input;
	{
		ZT_PROFILE_GAME("gameLoop:input");
		zt_inputKeysCopyState(input_keys);
		zt_inputMouseCopyState(&input_mouse);
		zt_inputControllerCopyState(&input_controller, 0);
		zt_inputGetKeyStrokes(input_keystrokes);
		input_this_frame = zt_inputThisFrame();

		if (input_keys[ztInputKeys_Control].pressed() && input_keys[ztInputKeys_Shift].pressed() && input_keys[ztInputKeys_Menu].pressed()) {
			if (input_keys[ztInputKeys_P].justPressed()) {
				if (zt_profilerIsPaused()) {
					zt_profilerResume();
				}
				else {
					zt_profilerPause();
				}
			}

			if (input_keys[ztInputKeys_D].justPressed()) {
				zt_guiDebugToggle();
			}
		}
#		if defined(ZT_INPUT_REPLAY)
		if (game->replay_state == ReplayState_Replaying) {
			if (input_keys[ztInputKeys_Space].justPressed()) {
				game->replay_state = ReplayState_Paused;
			}
		}
		else if (game->replay_state == ReplayState_Paused) {
			if (input_keys[ztInputKeys_Right].justPressedOrRepeated()) {
				game->replay_state = ReplayState_Stepping;
			}
			if (input_keys[ztInputKeys_Space].justPressed()) {
				game->replay_state = ReplayState_Replaying;
			}
		}

		if (game->replay_state != ReplayState_Paused) {
			if (!zt_inputReplayProcessFrame(&game->replay_data, game->details->current_frame, &input_this_frame, input_keys, &input_mouse, &input_controller, input_keystrokes)) {
				game->replaying = false;
				game->replay_state = ReplayState_None;
			}
#		else
		{
#		endif
			gui_input = zt_guiManagerHandleInput(game->gui_manager, input_keys, input_keystrokes, &input_mouse);
			if (!gui_input) {
				if (input_keys[ztInputKeys_Tilda].justPressed()) {
					bool console_shown = false;
					zt_debugConsoleToggle(&console_shown);
					if (console_shown) {
						zt_guiManagerSetKeyboardFocus(game->gui_manager);
					}
				}
			}
		}
	}

	if (game->game_scene_transition_to != GameScene_Invalid) {
		game->game_scene_transition_time -= dt;
		if (game->game_scene_transition_time < 0) {
			ZT_PROFILE_GAME("gameLoop:gamescene transition");
			game->game_scene_transition_time = 0;
			_gameSceneCleanup(game);

			GameScene_Enum transitioning_from = game->game_scene;
			game->game_scene = game->game_scene_transition_to;
			game->game_scene_transition_to = GameScene_Invalid;

			zt_tweenManagerReset(&game->tween_manager_state);

			if (game->gsf_begin[game->game_scene]) {
				if (!game->gsf_begin[game->game_scene](game, transitioning_from)) {
					return false;
				}
			}
		}
	}

	{
#		if defined(ZT_INPUT_REPLAY)
		if (game->replay_state != ReplayState_Paused)
#		endif
		{
			ZT_PROFILE_GAME("gameLoop:update");
			if (!game->gsf_update[game->game_scene](game, dt, gui_input, input_this_frame, input_keys, &input_controller, &input_mouse)) {
				return false;
			}
			zt_tweenManagerUpdate(&game->tween_manager_state, dt);
			zt_tweenManagerUpdate(&game->tween_manager_permanent, dt);
		}
	}

	{
		ZT_PROFILE_GAME("gameLoop:render state");
		zt_rendererClear(zt_vec4(0, 0, 0, 0));
		game->gsf_render[game->game_scene](game, game->render_target);

		// render ambient occlusion
		{
			{
				ZT_PROFILE_GAME("gameLoop:ao");
				static u32 position_tex_hash = zt_strHash("position_tex");
				static u32 normal_tex_hash   = zt_strHash("normal_tex");
				static u32 noise_tex_hash    = zt_strHash("noise_tex");
				static u32 cam_proj_hash     = zt_strHash("cam_proj");
		
				zt_shaderSetVariableTex (game->shader_ao, position_tex_hash, game->render_target_attach_position);
				zt_shaderSetVariableTex (game->shader_ao, normal_tex_hash,   game->render_target_attach_normal);
				zt_shaderSetVariableTex (game->shader_ao, noise_tex_hash,    game->texture_random);
				zt_shaderSetVariableMat4(game->shader_ao, cam_proj_hash,     game->camera_3d.mat_proj);
		
				zt_drawListAddScreenRenderTexture(&game->draw_list, game->render_target, &game->camera_2d, .5f, game->shader_ao);
				zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest, game->render_target_bright);
			}
			{
				ZT_PROFILE_GAME("gameLoop:ao_blur");
				static u32 texel_size_hash = zt_strHash("texel_size");

				zt_shaderSetVariableFloat(game->shader_hdr_bloom_blur_1, texel_size_hash, 1.f / zt_textureGetSize(game->render_target_bright).x * 1.f);
				zt_shaderSetVariableFloat(game->shader_hdr_bloom_blur_2, texel_size_hash, 1.f / zt_textureGetSize(game->render_target_bright).x * 1.f);

				int blur_iters = 2;
				zt_fiz(blur_iters) {
					zt_drawListAddScreenRenderTexture(&game->draw_list, game->render_target_bright, &game->camera_2d, .5f, game->shader_hdr_bloom_blur_1);
					zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest, game->render_target_blurred);

					zt_drawListAddScreenRenderTexture(&game->draw_list, game->render_target_blurred, &game->camera_2d, .5f, game->shader_hdr_bloom_blur_2);
					zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest, i == blur_iters - 1 ? game->render_target_ao : game->render_target_bright);
				}
			}
		}
		// draw the bright areas of the screen for bloom
		{
			ZT_PROFILE_GAME("gameLoop:bloom");
			zt_drawListAddScreenRenderTexture(&game->draw_list, game->render_target, &game->camera_2d, .5f, game->shader_hdr_bright);
			zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest, game->render_target_bright);

			static u32 texel_size_hash = zt_strHash("texel_size");
			zt_shaderSetVariableFloat(game->shader_hdr_bloom_blur_1, texel_size_hash, 1.f / zt_textureGetSize(game->render_target_bright).x * 1.f);
			zt_shaderSetVariableFloat(game->shader_hdr_bloom_blur_2, texel_size_hash, 1.f / zt_textureGetSize(game->render_target_bright).x * 1.f);

			zt_fiz(2) {
				zt_drawListAddScreenRenderTexture(&game->draw_list, game->render_target_bright, &game->camera_2d, .5f, game->shader_hdr_bloom_blur_1);
				zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest, game->render_target_blurred);

				zt_drawListAddScreenRenderTexture(&game->draw_list, game->render_target_blurred, &game->camera_2d, .5f, game->shader_hdr_bloom_blur_2);
				zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest, game->render_target_bright);
			}
		}
		static u32 bloom_tex_hash = zt_strHash("bloom_tex");
		static u32 ao_tex_hash    = zt_strHash("ao_tex");

		zt_shaderSetVariableTex  (game->shader_hdr_tonemap, bloom_tex_hash, game->render_target_blurred);
		zt_shaderSetVariableTex  (game->shader_hdr_tonemap, ao_tex_hash,    game->render_target_ao);

		zt_drawListAddScreenRenderTexture(&game->draw_list, game->render_target, &game->camera_2d, 1, game->shader_hdr_tonemap);

		if (game->fade_time > 0) {
			game->fade_time -= dt;
			if (game->fade_time < 0) {
				game->fade_time = 0;
			}

			r32 alpha = 1;
			r32 fade_time_half = game->fade_time_max / 2.f;
			if (game->fade_time < fade_time_half) {
				alpha = zt_linearRemapAndClamp(game->fade_time, 0, fade_time_half, 0, 1);
			}
			else {
				alpha = zt_linearRemapAndClamp(game->fade_time, fade_time_half, game->fade_time_max, 1, 0);
			}

			zt_drawListPushShader(&game->draw_list, zt_shaderGetDefault(ztShaderDefault_Unlit));
			zt_drawListAddSolidRect2D(&game->draw_list, ztVec3::zero, zt_vec2(128, 128), zt_color(0, 0, 0, alpha));
			zt_drawListPopShader(&game->draw_list);
		}

		zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest);
	}

	{
		ZT_PROFILE_GAME("gameLoop:render gui");

		zt_drawListPushShader(&game->draw_list, zt_shaderGetDefault(ztShaderDefault_Unlit));
		zt_guiManagerRender(game->gui_manager, &game->draw_list, dt);
		zt_drawListPopShader(&game->draw_list);

#		if defined(ZT_INPUT_REPLAY)
		if (game->replaying) {
			ztVec2 mpos = zt_cameraOrthoScreenToWorld(&game->camera_2d, input_mouse.screen_x, input_mouse.screen_y);
			r32 ppu = zt_pixelsPerUnit();

			zt_drawListPushShader(&game->draw_list, zt_shaderGetDefault(ztShaderDefault_Unlit));
			zt_drawListAddSolidCircle2D(&game->draw_list, mpos, 7 / ppu, 10, ztColor_DarkRed);
			zt_drawListAddSolidCircle2D(&game->draw_list, mpos, 2 / ppu, 10, ztColor_Red);

			ztVec2 ext = zt_cameraOrthoGetViewportSize(&game->camera_2d);

			char *text = "[Replaying]\nPress Space to Pause";
			if (game->replay_state == ReplayState_Paused || game->replay_state == ReplayState_Stepping) {
				text = "[Replay Paused]\nPress Space to Resume, Press -> to Step Forward One Frame";
			}

			zt_drawListAddText2D(&game->draw_list, ztFontDefault, text, zt_vec2(0, ext.y / -2 + .2f), ztAlign_Center | ztAlign_Bottom, ztAnchor_Center | ztAnchor_Bottom);
			zt_drawListPopShader(&game->draw_list);
		}
#		endif

		zt_renderDrawList(&game->camera_2d, &game->draw_list, ztVec4::zero, ztRenderDrawListFlags_NoDepthTest | ztRenderDrawListFlags_NoClear);
	}
	{
		ZT_PROFILE_GAME("gameLoop:asset check");
		zt_assetManagerCheckForChanges(&game->asset_manager);
	}

#	if defined(ZT_INPUT_REPLAY)
	{
		if (game->replay_state == ReplayState_Stepping) {
			game->replay_state = ReplayState_Paused;
		}
		else if (game->replay_state == ReplayState_Paused) {
			game->details->current_frame -= 1;
		}
	}
#	endif
	game->screen_updated = false;
	return true;
}

