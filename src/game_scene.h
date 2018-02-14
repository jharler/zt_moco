/**************************************************************************************************
 ** file: src\game_scene.h
 **
 ** This file was automatically generated.
 **************************************************************************************************/

#ifndef __game_scene_h__
#define __game_scene_h__

// headers ========================================================================================================================================================================================

#include "zt_game.h"

// types/enums/defines ============================================================================================================================================================================

enum GameScene_Enum
{
	GameScene_Invalid,
	GameScene_MainLoading,
	GameScene_Main,

	GameScene_MAX,
};

// ================================================================================================================================================================================================

struct ztGame;

// ================================================================================================================================================================================================

#define FUNC_GAME_SCENE_BEGIN(name)	    bool name(ztGame *game, GameScene_Enum transitioning_from)
typedef FUNC_GAME_SCENE_BEGIN(GameSceneBegin_Func);

#define FUNC_GAME_SCENE_CLEANUP(name)	void name(ztGame *game, GameScene_Enum transitioning_to)
typedef FUNC_GAME_SCENE_CLEANUP(GameSceneCleanup_Func);

#define FUNC_GAME_SCENE_UPDATE(name)	bool name(ztGame *game, r32 dt, bool gui_input, bool input_this_frame, ztInputKeys *input_keys, ztInputController *input_controller, ztInputMouse *input_mouse)
typedef FUNC_GAME_SCENE_UPDATE(GameSceneUpdate_Func);

#define FUNC_GAME_SCENE_RENDER(name)	void name(ztGame *game, ztTextureID final_render_target)
typedef FUNC_GAME_SCENE_RENDER(GameSceneRender_Func);

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

enum GameSceneSplashLoad_Enum
{
	GameSceneSplashLoad_Scene,

	GameSceneSplashLoad_MAX,
};

struct GameSceneSplashLoad
{
	ztLoadInfo                     load_info[GameSceneSplashLoad_MAX];
	int                            load_idx;
	r32                            load_time;

	// place variables required for loading here...
};


// ================================================================================================================================================================================================

struct GameSceneSplash
{
	GameSceneSplashLoad load;
	r32                 time;
};


FUNC_GAME_SCENE_BEGIN   (gameSceneSplashLoadingBegin);
FUNC_GAME_SCENE_CLEANUP (gameSceneSplashLoadingCleanup);
FUNC_GAME_SCENE_UPDATE  (gameSceneSplashLoadingUpdate);
FUNC_GAME_SCENE_RENDER  (gameSceneSplashLoadingRender);

FUNC_GAME_SCENE_BEGIN   (gameSceneSplashBegin);
FUNC_GAME_SCENE_CLEANUP (gameSceneSplashCleanup);
FUNC_GAME_SCENE_UPDATE  (gameSceneSplashUpdate);
FUNC_GAME_SCENE_RENDER  (gameSceneSplashRender);

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

struct GameSceneMenu
{
};


FUNC_GAME_SCENE_BEGIN   (gameSceneMenuBegin);
FUNC_GAME_SCENE_CLEANUP (gameSceneMenuCleanup);
FUNC_GAME_SCENE_UPDATE  (gameSceneMenuUpdate);
FUNC_GAME_SCENE_RENDER  (gameSceneMenuRender);

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

enum GameSceneMainLoad_Enum
{
	GameSceneMainLoad_Scene,

	GameSceneMainLoad_MAX,
};

struct GameSceneMainLoad
{
	ztLoadInfo                     load_info[GameSceneMainLoad_MAX];
	int                            load_idx;
	r32                            load_time;

	// place variables required for loading here...
};

// ================================================================================================================================================================================================

struct GameSceneMain
{
	ztGame                        *game;
	GameSceneMainLoad              load;

	ztScene                       *scene;
	ztLight                        scene_light;
	ztCameraControllerArcball      camera_controller;
	ztModel                        models[128];
	int                            models_used;
	ztLight                        lights[9];
	int                            lights_used;

};

// ================================================================================================================================================================================================

FUNC_GAME_SCENE_BEGIN   (gameSceneMainLoadingBegin);
FUNC_GAME_SCENE_CLEANUP (gameSceneMainLoadingCleanup);
FUNC_GAME_SCENE_UPDATE  (gameSceneMainLoadingUpdate);
FUNC_GAME_SCENE_RENDER  (gameSceneMainLoadingRender);

FUNC_GAME_SCENE_BEGIN   (gameSceneMainBegin);
FUNC_GAME_SCENE_CLEANUP (gameSceneMainCleanup);
FUNC_GAME_SCENE_UPDATE  (gameSceneMainUpdate);
FUNC_GAME_SCENE_RENDER  (gameSceneMainRender);

// used by serialization functions using ztLoadState.  allows for easy error handling while checking every read/write call
// there must be an on_error label that sets the ztLoadState state to error
#define _serialValidate(system, cond, saving)           if(!(cond)) { zt_logCritical("serial: %s : Unable to %s file(%d)", system, ((saving) ? "write to" : "read from"), __LINE__); goto on_error; }
#define _serialValidateMsg(system, cond, error)         if(!(cond)) { zt_logCritical("serial: %s: %s (%d)", system, error, __LINE__); goto on_error; }

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

#endif // include guard
