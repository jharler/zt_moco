/**************************************************************************************************
 ** file: src\gui_theme.h
 **
 ** This file was automatically generated.
 **************************************************************************************************/

/**************************************************************************************************************************************************************************************************
** file: src/gui_theme.h
**
 ** This file was automatically generated.
***************************************************************************************************************************************************************************************************/

#ifndef __gui_theme_h__
#define __gui_theme_h__

// ================================================================================================================================================================================================

#include "zt_game_gui.h"

// ================================================================================================================================================================================================

enum GuiThemeItemFlags_Enum
{
	// use these to identify different controls
	GuiThemeItemFlags_        = (1<<0),
};

enum GuiThemeStates_Enum
{
	// use these to give gui controls custom states
	GuiThemeStates_ = ztGuiItemStates_USER,
};

// ================================================================================================================================================================================================

struct ztGame;

// ================================================================================================================================================================================================

enum GuiSpriteNineSlice_Enum
{
	GuiSpriteNineSlice_WindowFrame,
	GuiSpriteNineSlice_WindowFrameDisabled,
	GuiSpriteNineSlice_PanelFrame,
	GuiSpriteNineSlice_PanelFrameDisabled,
	GuiSpriteNineSlice_CollapsingPanelFrame,
	GuiSpriteNineSlice_CollapsingPanelFrameDisabled,
	GuiSpriteNineSlice_ButtonNormal,
	GuiSpriteNineSlice_ButtonHighlight,
	GuiSpriteNineSlice_ButtonPressed,
	GuiSpriteNineSlice_ButtonDisabled,
	GuiSpriteNineSlice_ButtonToggleNormal,
	GuiSpriteNineSlice_ButtonToggleHighlight,
	GuiSpriteNineSlice_ButtonTogglePressed,
	GuiSpriteNineSlice_ButtonToggleDisabled,
	GuiSpriteNineSlice_ButtonToggleOn,
	GuiSpriteNineSlice_ButtonToggleOnDisabled,
	GuiSpriteNineSlice_Menu,
	GuiSpriteNineSlice_MenuBar,
	GuiSpriteNineSlice_MenuBarDisabled,
	GuiSpriteNineSlice_TextEdit,
	GuiSpriteNineSlice_TextEditDisabled,
	GuiSpriteNineSlice_ScrollbarVertBackground,
	GuiSpriteNineSlice_ScrollbarVertBackgroundDisabled,
	GuiSpriteNineSlice_ScrollbarVertHandle,
	GuiSpriteNineSlice_ScrollbarVertHandleHighlight,
	GuiSpriteNineSlice_ScrollbarVertHandlePressed,
	GuiSpriteNineSlice_ScrollbarVertHandleDisabled,
	GuiSpriteNineSlice_ScrollbarHorzBackground,
	GuiSpriteNineSlice_ScrollbarHorzBackgroundDisabled,
	GuiSpriteNineSlice_ScrollbarHorzHandle,
	GuiSpriteNineSlice_ScrollbarHorzHandleHighlight,
	GuiSpriteNineSlice_ScrollbarHorzHandlePressed,
	GuiSpriteNineSlice_ScrollbarHorzHandleDisabled,
	GuiSpriteNineSlice_SliderVertBar,
	GuiSpriteNineSlice_SliderVertBarDisabled,
	GuiSpriteNineSlice_SliderHorzBar,
	GuiSpriteNineSlice_SliderHorzBarDisabled,
	GuiSpriteNineSlice_ComboBox,
	GuiSpriteNineSlice_ComboBoxHighlight,
	GuiSpriteNineSlice_ComboBoxPressed,
	GuiSpriteNineSlice_ComboBoxDisabled,
	GuiSpriteNineSlice_CycleBoxBody,
	GuiSpriteNineSlice_CycleBoxBodyDisabled,
	GuiSpriteNineSlice_CycleBoxLeft,
	GuiSpriteNineSlice_CycleBoxLeftHighlight,
	GuiSpriteNineSlice_CycleBoxLeftPressed,
	GuiSpriteNineSlice_CycleBoxLeftDisabled,
	GuiSpriteNineSlice_CycleBoxRight,
	GuiSpriteNineSlice_CycleBoxRightHighlight,
	GuiSpriteNineSlice_CycleBoxRightPressed,
	GuiSpriteNineSlice_CycleBoxRightDisabled,
	GuiSpriteNineSlice_Solid,

	GuiSpriteNineSlice_MAX,
};

// ================================================================================================================================================================================================

enum GuiSprite_Enum
{
	GuiSprite_CheckBox,
	GuiSprite_CheckBoxHighlight,
	GuiSprite_CheckBoxPressed,
	GuiSprite_CheckBoxDisabled,
	GuiSprite_Radio,
	GuiSprite_RadioHighlight,
	GuiSprite_RadioPressed,
	GuiSprite_RadioDisabled,
	GuiSprite_RadioSelect,
	GuiSprite_SliderHorzHandle,
	GuiSprite_SliderHorzHandleHighlight,
	GuiSprite_SliderHorzHandlePressed,
	GuiSprite_SliderHorzHandleDisabled,
	GuiSprite_SliderVertHandle,
	GuiSprite_SliderVertHandleHighlight,
	GuiSprite_SliderVertHandlePressed,
	GuiSprite_SliderVertHandleDisabled,
	GuiSprite_Resizer,
	GuiSprite_HorzLine,
	GuiSprite_IconPlus,
	GuiSprite_IconMinus,
	GuiSprite_IconArrowLeft,
	GuiSprite_IconArrowRight,
	GuiSprite_IconArrowUp,
	GuiSprite_IconArrowDown,
	GuiSprite_IconHandGrab,
	GuiSprite_IconHandPoint,
	GuiSprite_IconPencil,
	GuiSprite_IconTrash,
	GuiSprite_IconUndo,
	GuiSprite_IconCopy,
	GuiSprite_IconCut,
	GuiSprite_IconNew,
	GuiSprite_IconSave,
	GuiSprite_IconOpen,
	GuiSprite_IconWarning,
	GuiSprite_IconMenu,
	GuiSprite_IconCopyright,
	GuiSprite_IconCheck,
	GuiSprite_IconX,
	GuiSprite_IconGear,
	GuiSprite_IconCancel,
	GuiSprite_IconPlay,
	GuiSprite_IconPause,
	GuiSprite_IconRewind,
	GuiSprite_IconFastForward,
	GuiSprite_IconBegin,
	GuiSprite_IconEnd,

	GuiSprite_MAX,
};

// ================================================================================================================================================================================================

enum GuiAudio_Enum
{
	GuiAudio_,

	GuiAudio_MAX,
};

// ================================================================================================================================================================================================

struct GuiThemeData
{
	ztTextureID        texture;

	ztSpriteManager    sprite_manager;

	ztSpriteNineSlice *sprite_nine_slice[GuiSpriteNineSlice_MAX];
	ztSprite          *sprite           [GuiSprite_MAX];
	ztAudioClipID      audio            [GuiAudio_MAX];

	ztGame            *game;
	ztCamera          *gui_camera;

	ztFontID           font;
	ztFontID           font_monospace;
};

// ================================================================================================================================================================================================

bool guiThemeMake(ztGuiTheme *theme, ztAssetManager *asset_manager, ztGame *game);
void guiThemeFree(ztGuiTheme *theme);

// ================================================================================================================================================================================================

#endif // include guard
