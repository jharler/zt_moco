/**************************************************************************************************
 ** file: src\unity_dll.cpp
 **
 ** This file was automatically generated.
 **************************************************************************************************/

#if !defined(ZT_DLL) && !defined(ZT_NO_LOADER)
#define ZT_DLL
#endif

#ifndef ZT_NO_DIRECTX
#define ZT_NO_DIRECTX
#endif

//#define ZT_OPENGL_DEBUGGING
//#define ZT_NO_PROFILE

// ================================================================================================================================================================================================
// All source files must be included here.
// Remove them from compilation if they are added to the project
// After generation, you may need to edit this file to trigger MSVC's file change detection in the included files

#include "main_dll.cpp"
#include "game_scene_main.cpp"
#include "gui_theme.cpp"
#include "moco.cpp"


// ================================================================================================================================================================================================

#ifndef ZT_LOADER

#define ZT_TOOLS_IMPLEMENTATION
#include "zt_tools.h"

#define ZT_GAME_IMPLEMENTATION
#include "zt_game.h"

#define ZT_GAME_GUI_IMPLEMENTATION
#include "zt_game_gui.h"

#endif
