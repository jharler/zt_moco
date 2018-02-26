/**************************************************************************************************
 ** file: src\game.h
 **
 ** This file was automatically generated.
 **************************************************************************************************/

#ifndef __game_h__
#define __game_h__

// headers ========================================================================================================================================================================================

#include "zt_tools.h"
#include "zt_game.h"
#include "zt_game_gui.h"
#include "gui_theme.h"
#include "game_scene.h"


// types/enums/defines ============================================================================================================================================================================

#if defined(ZT_INPUT_REPLAY)
enum ReplayState_Enum
{
	ReplayState_None,
	ReplayState_Recording,
	ReplayState_Replaying,
	ReplayState_Stepping,
	ReplayState_Paused,

	ReplayState_MAX,
};
#endif

#define CAM_START_POS		zt_vec3(0, 12, 12)
#define CAM_START_ROT		ztQuat::makeFromEuler(44, 0, 0)

// structs/classes ================================================================================================================================================================================

// NOTE: do not assign values in the struct definition
//       can cause problem with the stack when this struct grows large

struct ztGame
{
	ztGameDetails         *details;
	ztGameSettings        *settings;

#	if defined(ZT_INPUT_REPLAY)
	ztInputReplayData      replay_data;
	bool                   replaying;
	ReplayState_Enum       replay_state;
#	endif

	ztCamera               camera_2d;
	ztCamera               camera_3d;
	ztGuiManager          *gui_manager;
	ztGuiTheme             gui_theme;
	GuiThemeData          *gui_theme_data;
	ztAssetManager         asset_manager;
	ztDrawList             draw_list;
	GameScene_Enum         game_scene;
	GameScene_Enum         game_scene_transition_to;
	r32                    game_scene_transition_time;
	GameSceneSplash        game_scene_splash;
	GameSceneMenu          game_scene_menu;
	GameSceneMain          game_scene_main;

	GameSceneBegin_Func   *gsf_begin  [GameScene_MAX];
	GameSceneCleanup_Func *gsf_cleanup[GameScene_MAX];
	GameSceneUpdate_Func  *gsf_update [GameScene_MAX];
	GameSceneRender_Func  *gsf_render [GameScene_MAX];

	ztTextureID            render_target;

	ztTextureID            render_target_attach_position;
	ztTextureID            render_target_attach_normal;

	ztTextureID            render_target_bright;
	ztTextureID            render_target_blurred;
	ztTextureID            texture_random;
	ztTextureID            render_target_ao;

	ztShaderID             shader_hdr_tonemap;
	ztShaderID             shader_hdr_bright;
	ztShaderID             shader_hdr_bloom_blur_1;
	ztShaderID             shader_hdr_bloom_blur_2;
	ztShaderID             shader_ao;

	r32                    fade_time;
	r32                    fade_time_max;

	ztTweenManager         tween_manager_state;
	ztTweenManager         tween_manager_permanent;

	bool                   screen_updated;

	ztGuiItem             *toolbar;
};


// external variables =============================================================================================================================================================================

// function prototypes ============================================================================================================================================================================

void gameAdjustCamera(ztGame *game);


// inline functions ===============================================================================================================================================================================

#endif // include guard
