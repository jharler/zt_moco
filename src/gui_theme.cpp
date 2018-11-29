/**************************************************************************************************
 ** file: src\gui_theme.cpp
 **
 ** This file was automatically generated.
 **************************************************************************************************/

/**************************************************************************************************************************************************************************************************
** file: src/gui_theme.cpp
**
** Responsible for drawing the gui elements
***************************************************************************************************************************************************************************************************/

#define ZT_GAME_GUI_INTERNAL_DECLARATIONS
#include "zt_game_gui.h"

#include "gui_theme.h"
#include "game.h"

// ================================================================================================================================================================================================

#define ZT_GUI_HIGHLIGHT_COLOR	zt_color(1.f, .5f, 0.f, .3f)

// ================================================================================================================================================================================================

ztInternal bool _guiThemeIsCustom(ztGuiItem *item)
{
	if (zt_guiItemGetTopLevelParent(item)->custom_flags == 0) {
	}

	return true;
}

// ================================================================================================================================================================================================

ZT_FUNC_THEME_GET_RVALUE(guiThemeGetRValue)
{
	ZT_PROFILE_GUI("guiThemeGetRValue");

	if (!_guiThemeIsCustom(item)) {
		return false;
	}

	r32 ppu = zt_pixelsPerUnit();

	switch (value)
	{
		case ztGuiThemeValue_r32_Padding:                     *result = 6 / ppu; break;

		case ztGuiThemeValue_r32_WindowTitleHeight:           *result = 24 / ppu; break;
		case ztGuiThemeValue_r32_WindowCollapseButtonOffsetX: *result = 5 / ppu; break;
		case ztGuiThemeValue_r32_WindowCollapseButtonOffsetY: *result = -1 / ppu; break;
		case ztGuiThemeValue_r32_WindowCloseButtonOffsetX:    *result = -5 / ppu; break;
		case ztGuiThemeValue_r32_WindowCloseButtonOffsetY:    *result = -1 / ppu; break;
		case ztGuiThemeValue_r32_WindowPaddingX:              *result = 3 / ppu; break;
		case ztGuiThemeValue_r32_WindowPaddingY:              *result = 3 / ppu; break;
		case ztGuiThemeValue_r32_WindowCornerResizeW:         *result = 16 / ppu; break;
		case ztGuiThemeValue_r32_WindowCornerResizeH:         *result = 16 / ppu; break;

		case ztGuiThemeValue_r32_CheckboxW:                   *result = 20 / ppu; break;
		case ztGuiThemeValue_r32_CheckboxH:                   *result = 20 / ppu; break;

		case ztGuiThemeValue_r32_SliderHandleSize:            *result = 10 / ppu; break;
		case ztGuiThemeValue_r32_SliderHandleMinHeight:       *result = 10 / ppu; break;

		case ztGuiThemeValue_r32_ScrollbarButtonW:            *result = 0 / ppu; break;
		case ztGuiThemeValue_r32_ScrollbarButtonH:            *result = 0 / ppu; break;
		case ztGuiThemeValue_r32_ScrollbarMinHandleSize:      *result = 10 / ppu; break;
		case ztGuiThemeValue_r32_ScrollbarMinWidth:           *result = 10 / ppu; break;

		case ztGuiThemeValue_r32_ComboboxButtonW:             *result = 27 / ppu; break;

		case ztGuiThemeValue_r32_CycleBoxButtonW:             *result = 27 / ppu; break;

		default: return false;  // return false to use the default
	}

	return true;
}

// ------------------------------------------------------------------------------------------------

ZT_FUNC_THEME_GET_IVALUE(guiThemeGetIValue)
{
	ZT_PROFILE_GUI("guiThemeGetIValue");

	if (!_guiThemeIsCustom(item)) {
		return false;
	}

	GuiThemeData *theme_data = (GuiThemeData*)theme->theme_data;

	switch (value)
	{
		case ztGuiThemeValue_i32_WindowCloseButtonAlign:            *result = ztAlign_Right | ztAlign_Top; break;
		case ztGuiThemeValue_i32_WindowCloseButtonAnchor:           *result = ztAnchor_Right | ztAnchor_Top; break;
		case ztGuiThemeValue_i32_WindowCloseButtonBehaviorFlags:    *result = ztGuiButtonBehaviorFlags_NoBackground | ztGuiButtonBehaviorFlags_OnPressDip; break;


		case ztGuiThemeValue_i32_MenuFontID: {
			*result = theme_data->font;
			return true;
		} break;

		default: return false;  // return false to use the default
	}

	return true;
}

// ------------------------------------------------------------------------------------------------

ZT_FUNC_THEME_UPDATE_ITEM(guiThemeUpdateItem)
{
	ZT_PROFILE_GUI("guiThemeUpdateItem");

	r32 ppu = zt_pixelsPerUnit();

	//	switch (item->type)
	//	{
	//
	//		default: return true;  // return false to use the default (this is the default though)
	//	}

	return false;
}

// ------------------------------------------------------------------------------------------------

ZT_FUNC_THEME_UPDATE_SUBITEM(guiThemeUpdateSubitem)
{
	ZT_PROFILE_GUI("guiThemeUpdateSubitem");

	if (!_guiThemeIsCustom(item)) {
		return false;
	}
	GuiThemeData *theme_data = (GuiThemeData*)theme->theme_data;
	r32 ppu = zt_pixelsPerUnit();

	if (item->guid == ZT_GUI_WINDOW_GUID) {
		if (subitem->guid == ZT_GUI_BUTTON_GUID) {
			char *name = zt_guiItemGetName(subitem);

			if (zt_strEquals(name, "Close")) {
				zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconX]);
			}
			else if (zt_strEquals(name, "Collapse")) {
				if (zt_bitIsSet(item->state_flags, zt_bit(ztGuiWindowInternalStates_Collapsed))) {
					zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconArrowRight]);
				}
				else {
					zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconArrowDown]);
				}
			}
			zt_guiItemSetSize(subitem, zt_vec2(26 / ppu, 26 / ppu));
		}
	} 

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_COLLAPSING_PANEL_GUID) {
		if (subitem->guid == ZT_GUI_BUTTON_GUID) {
			if (zt_bitIsSet(item->state_flags, zt_bit(ztGuiCollapsingPanelInternalStates_Collapsed))) {
				zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconArrowRight]);
			}
			else {
				zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconArrowDown]);
			}
			zt_guiItemSetSize(subitem, zt_vec2(21 / ppu, 21 / ppu));
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_TREE_GUID) {
		if (subitem->guid == ZT_GUI_BUTTON_GUID) {
			if (data == nullptr || *((bool*)data) == true) {
				zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconMinus]);
			}
			else {
				zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconPlus]);
			}
			zt_guiItemSetSize(subitem, zt_vec2(17 / ppu, 17 / ppu));
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_CYCLEBOX_GUID) {
		r32 button_w = zt_guiThemeGetRValue(zt_guiItemGetTheme(item), ztGuiThemeValue_r32_CycleBoxButtonW, item);

		subitem->size.x = button_w;
		subitem->size.y = item->size.y;

		if (subitem->guid == ZT_GUI_BUTTON_GUID) {
			subitem->behavior_flags |= ztGuiButtonBehaviorFlags_NoBackground;

			if (((ztDirection_Enum)(i64)data) == ztDirection_Left) {
				zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconArrowLeft]);
				zt_guiItemSetPosition(subitem, ztAlign_Left, ztAnchor_Left, ztVec2::zero);
			}
			else if (((ztDirection_Enum)(i64)data) == ztDirection_Right) {
				zt_guiButtonSetIcon(subitem, theme_data->sprite[GuiSprite_IconArrowRight]);
				zt_guiItemSetPosition(subitem, ztAlign_Right, ztAnchor_Right, ztVec2::zero);
			}
		}
	}

	// ================================================================================================================================================================================================

	else {
		return false; // return false to use the default
	}

	return true;
}

// ================================================================================================================================================================================================

ZT_FUNC_THEME_SIZE_ITEM(guiThemeSizeItem)
{
	ZT_PROFILE_GUI("guiThemeSizeItem");

	if (!_guiThemeIsCustom(item)) {
		return false;
	}

	GuiThemeData *theme_data = (GuiThemeData*)theme->theme_data;

	r32 ppu = zt_pixelsPerUnit();

	if (item->guid == ZT_GUI_BUTTON_GUID) {
		ztGuiButtonState *button_state = (ztGuiButtonState*)item->state;
		ztVec2 icon_size = button_state->icon ? button_state->icon->half_size * 2.f : ztVec2::zero;
		ztFontID font = theme_data->font;
		item->size = zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_Fancy) ? zt_fontGetExtentsFancy(font, item->label) : zt_fontGetExtents(font, item->label);
		item->size.x = zt_max(icon_size.x, item->size.x);
		item->size.y += icon_size.y;

		item->size += zt_vec2(21 / ppu, 21 / ppu);
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_STATIC_TEXT_GUID) {
		ztFontID font = zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_MonoSpaced) ? theme_data->font_monospace : theme_data->font;
		item->size = zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_Fancy) ? zt_fontGetExtentsFancy(font, item->label) : zt_fontGetExtents(font, item->label);
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_SLIDER_GUID) {
		ztGuiSliderState *slider_state = (ztGuiSliderState*)item->state;
		if (slider_state->orient == ztGuiItemOrient_Horz) {
			item->size = zt_vec2(5, zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_SliderHandleSize, item));
		}
		else {
			item->size = zt_vec2(zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_SliderHandleSize, item), 5);
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_CHECKBOX_GUID || item->guid == ZT_GUI_RADIO_BUTTON_GUID) {
		ztVec2 txt_size = zt_fontGetExtents(theme_data->font, item->label);
		r32 padding     = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);
		r32 checkbox_w  = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_CheckboxW, item);
		r32 checkbox_h  = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_CheckboxW, item);
		item->size.x = txt_size.x + padding + checkbox_w;
		item->size.y = zt_max(txt_size.y, checkbox_h);
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_COMBOBOX_GUID) {
		r32 base_width = 38 / ppu;

		item->size.x = 160 / ppu;
		item->size.y = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_ComboboxButtonW, item);

		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);
		ztFontID font = theme_data->font;

		ztGuiComboboxState *combobox_state = (ztGuiComboboxState*)item->state;
		zt_fiz(combobox_state->contents_count) {
			ztVec2 ext = zt_fontGetExtents(font, combobox_state->contents[i]);
			item->size.x = zt_max(item->size.x, base_width + ext.x);
			item->size.y = zt_max(item->size.y, ext.y + padding * 2);
		}
	} 

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_MENU_GUID) {
		ztVec2 icon_orig = zt_vec2(zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconX, item), zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconY, item));
		ztVec2 icon = icon_orig;
		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);

		ztGuiMenuState *menu_state = (ztGuiMenuState*)item->state;
		zt_fiz(menu_state->item_count) {
			if (menu_state->icons[i] != nullptr) {
				icon.x = zt_max(icon.x, menu_state->icons[i]->half_size.x * 2.f + padding * 2.f);
				icon.y = zt_max(icon.y, menu_state->icons[i]->half_size.y * 2.f);
			}
		}

		item->size = ztVec2::zero;

		ztFontID font = theme_data->font;

		zt_fiz(menu_state->item_count) {
			ztVec2 ext = zt_fontGetExtents(font, menu_state->display[i]);

			if (menu_state->ids[i] == ztInvalidID && zt_strStartsWith(menu_state->display[i], "__")) {
				ext.y = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSeparatorHeight, item);
			}

			item->size.y += zt_max(zt_max(icon.y, ext.y), icon_orig.y) + padding;
			item->size.x = zt_max(item->size.x, ext.x + icon.x + icon_orig.x + padding * 3.f);
		}
		item->size.y += padding;
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_CYCLEBOX_GUID) {
		r32 base_width = 144 / ppu;

		item->size.x = 180 / ppu;
		item->size.y = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_TextEditDefaultH, item);

		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);
		ztFontID font = theme_data->font;

		ztGuiCycleboxState *cyclebox_state = (ztGuiCycleboxState*)item->state;

		zt_fiz(cyclebox_state->contents_count) {
			ztVec2 ext = zt_fontGetExtents(font, cyclebox_state->contents[i]);
			item->size.x = zt_max(item->size.x, base_width + ext.x);
			item->size.y = zt_max(item->size.y, ext.y + padding * 2);
		}
	}

	// ================================================================================================================================================================================================

	else {
		return false; // return false to use the default
	}
	
	return true;
}

// ================================================================================================================================================================================================

ZT_FUNC_THEME_RENDER_ITEM(guiThemeRenderItem)
{
	ZT_PROFILE_GUI("guiThemeRenderItem");

	GuiThemeData *theme_data = (GuiThemeData*)theme->theme_data;

	r32 ppu = zt_pixelsPerUnit();

	bool has_focus   = zt_bitIsSet(item->state_flags, zt_bit(ztGuiItemStates_HasFocus));
	bool highlighted = zt_bitIsSet(item->gm->item_cache_flags[item->id], ztGuiManagerItemCacheFlags_MouseOver);
	bool pressed     = (highlighted && item->gm->item_has_mouse == item) || (!highlighted && zt_bitIsSet(item->state_flags, zt_bit(ztGuiButtonInternalStates_IsToggled)));
	bool enabled     = !zt_bitIsSet(item->state_flags, zt_bit(ztGuiItemStates_Disabled));

	struct local
	{
		static i32 imageIndex(i32 img_normal, i32 img_highlight, i32 img_pressed, i32 img_disabled, ztGuiItem *item)
		{
			if (img_highlight < 0) img_highlight = img_normal;
			if (img_pressed < 0) img_pressed = img_normal;
			if (img_disabled < 0) img_pressed = img_normal;

			bool disabled = zt_bitIsSet(item->state_flags, zt_bit(ztGuiItemStates_Disabled));
			ztGuiItem *parent = item->parent;
			while (!disabled && parent) {
				if (zt_bitIsSet(parent->state_flags, zt_bit(ztGuiItemStates_Disabled))) {
					disabled = true;
					break;
				}
				parent = parent->parent;
			}

			if (disabled) return img_disabled;

			bool highlighted = zt_bitIsSet(item->gm->item_cache_flags[item->id], ztGuiManagerItemCacheFlags_MouseOver);
			bool pressed = (highlighted && item->gm->item_has_mouse == item);

			if (pressed) return img_pressed;
			if (highlighted) return img_highlight;

			return img_normal;
		}
	};

	if (item->guid == ZT_GUI_WINDOW_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:Window");

		if (zt_bitIsSet(item->behavior_flags, ztGuiWindowBehaviorFlags_Modal)) {
			zt_drawListPushColor(draw_list, zt_color(0,0,0, .75f));
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid], ztVec2::zero, zt_cameraOrthoGetViewportSize(item->gm->gui_camera));
			zt_drawListPopColor(draw_list);
		}

		if (zt_bitIsSet(item->behavior_flags, ztGuiWindowBehaviorFlags_ShowTitle)) {
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_WindowFrame, -1, -1, GuiSpriteNineSlice_WindowFrameDisabled, item)], item->pos, item->size);
			zt_drawListAddText2D(draw_list, theme_data->font, item->label, zt_vec2(pos.x, pos.y + item->size.y / 2 - 6 / ppu), ztAlign_Center | ztAlign_Top, ztAnchor_Center | ztAnchor_Top);
		}
		else {
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_PanelFrame, -1, -1, GuiSpriteNineSlice_PanelFrameDisabled, item)], pos, item->size);
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_PANEL_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:Window");

		if (zt_bitIsSet(item->behavior_flags, ztGuiPanelBehaviorFlags_DrawBackground)) {
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_PanelFrame, -1, -1, GuiSpriteNineSlice_PanelFrameDisabled, item)], pos, item->size);
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_COLLAPSING_PANEL_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:CollapsingPanel");

		if (zt_bitIsSet(item->state_flags, zt_bit(ztGuiCollapsingPanelInternalStates_Collapsed))) {
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_PanelFrame, -1, -1, GuiSpriteNineSlice_PanelFrameDisabled, item)], pos, item->size);
		}
		else {
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_CollapsingPanelFrame, -1, -1, GuiSpriteNineSlice_CollapsingPanelFrameDisabled, item)], pos, item->size);
		}

		if (item->label != nullptr) {
			r32 panel_height = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_CollapsingPanelHeight, item);

			ztVec2 title_pos = zt_vec2(pos.x - ((item->size.x) / 2) + 25 / ppu, pos.y + (item->size.y - (panel_height)) / 2);
			ztVec2 title_size;

			//				if (!zt_bitIsSet(item->state_flags, zt_bit(ztGuiCollapsingPanelInternalStates_Collapsed)) {
			//				}

			zt_drawListAddText2D(draw_list, 0, item->label, title_pos, ztAlign_Left, ztAnchor_Left, &title_size);
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_TOGGLE_BUTTON_GUID || item->guid == ZT_GUI_BUTTON_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:Button");
		ztFontID font = theme_data->font;
		r32 scale = 1;

		if (font != ztInvalidID) {
			ztVec2 ext = zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_Fancy) ? zt_fontGetExtentsFancy(font, item->label) : zt_fontGetExtents(font, item->label);
			ztVec2 off = pos;

			ztGuiButtonState *button_state = (ztGuiButtonState*)item->state;

			if (button_state->icon) {
				off.y -= button_state->icon->half_size.y * 1;
			}

			ext *= scale;

			if (item->align_flags != 0) {
				if (zt_bitIsSet(item->align_flags, ztAlign_Left)) off.x -= (item->size.x - ext.x) / 2.f;
				if (zt_bitIsSet(item->align_flags, ztAlign_Right)) off.x += (item->size.x - ext.x) / 2.f;
				if (zt_bitIsSet(item->align_flags, ztAlign_Top)) off.y += (item->size.y - ext.y) / 2.f;
				if (zt_bitIsSet(item->align_flags, ztAlign_Bottom)) off.y -= (item->size.y - ext.y) / 2.f;
			}

			ztVec2 text_ext = ztVec2::zero;

			if (!zt_bitIsSet(item->behavior_flags, ztGuiButtonBehaviorFlags_NoBackground) || highlighted) {
				if (item->guid == ZT_GUI_TOGGLE_BUTTON_GUID) {
					bool toggled = zt_bitIsSet(item->state_flags, zt_bit(ztGuiButtonInternalStates_IsToggled));
					zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(toggled ? GuiSpriteNineSlice_ButtonToggleOn : GuiSpriteNineSlice_ButtonToggleNormal, GuiSpriteNineSlice_ButtonToggleHighlight, GuiSpriteNineSlice_ButtonTogglePressed, toggled ? GuiSpriteNineSlice_ButtonToggleOnDisabled : GuiSpriteNineSlice_ButtonToggleDisabled, item)], pos, item->size);
				}
				else {
					zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_ButtonNormal, GuiSpriteNineSlice_ButtonHighlight, GuiSpriteNineSlice_ButtonPressed, GuiSpriteNineSlice_ButtonDisabled, item)], pos, item->size);
				}
			}

			if (zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_Fancy)) {
				zt_drawListAddFancyText2D(draw_list, font, item->label, off, zt_vec2(scale, scale), item->align_flags, item->anchor_flags, &text_ext);
			}
			else {
				zt_drawListAddText2D(draw_list, font, item->label, off, zt_vec2(scale, scale), item->align_flags, item->anchor_flags, &text_ext);
			}

			if (button_state->icon) {
				if (text_ext.y > 0) {
					off.y += 3 / ppu;
				}

				ztVec3 icon_pos = zt_vec3(off + zt_vec2(0, text_ext.y / 2.f + button_state->icon->half_size.y), 0);
				zt_drawListAddSpriteFast(draw_list, button_state->icon, icon_pos, ztVec3::zero, zt_vec3(scale, scale, scale));
			}
		}
		else return false;
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_STATIC_TEXT_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:StaticText");
		ztFontID font = zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_MonoSpaced) ? theme_data->font_monospace : theme_data->font;
		r32 scale = 1;

		if (font != ztInvalidID) {
			ztVec2 ext = zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_Fancy) ? zt_fontGetExtentsFancy(font, item->label) : zt_fontGetExtents(font, item->label);
			ztVec2 off = pos;


			ext *= scale;

			if (item->align_flags != 0) {
				if (zt_bitIsSet(item->align_flags, ztAlign_Left)) off.x -= (item->size.x - ext.x) / 2.f;
				if (zt_bitIsSet(item->align_flags, ztAlign_Right)) off.x += (item->size.x - ext.x) / 2.f;
				if (zt_bitIsSet(item->align_flags, ztAlign_Top)) off.y += (item->size.y - ext.y) / 2.f;
				if (zt_bitIsSet(item->align_flags, ztAlign_Bottom)) off.y -= (item->size.y - ext.y) / 2.f;
			}

			ztVec2 text_ext = ztVec2::zero;

			if (zt_bitIsSet(item->behavior_flags, ztGuiStaticTextBehaviorFlags_Fancy)) {
				zt_drawListAddFancyText2D(draw_list, font, item->label, off, zt_vec2(scale, scale), item->align_flags, item->anchor_flags, &text_ext);
			}
			else {
				zt_drawListAddText2D(draw_list, font, item->label, off, zt_vec2(scale, scale), item->align_flags, item->anchor_flags, &text_ext);
			}
		}
		else return false;
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_CHECKBOX_GUID || item->guid == ZT_GUI_RADIO_BUTTON_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:CheckRadio");

		bool checkbox = zt_bitIsSet(item->behavior_flags, ztGuiButtonInternalBehaviorFlags_IsCheckbox);
		bool radio    = zt_bitIsSet(item->behavior_flags, ztGuiButtonInternalBehaviorFlags_IsRadio);

		ztGuiButtonState *checkbox_state = (ztGuiButtonState*)item->state;

		r32 checkbox_w = zt_guiThemeGetRValue(theme, item->guid == ZT_GUI_CHECKBOX_GUID ? ztGuiThemeValue_r32_CheckboxW : ztGuiThemeValue_r32_RadiobuttonW, item);
		r32 checkbox_h = zt_guiThemeGetRValue(theme, item->guid == ZT_GUI_CHECKBOX_GUID ? ztGuiThemeValue_r32_CheckboxW : ztGuiThemeValue_r32_RadiobuttonW, item);
		r32 padding    = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);

		ztVec2 box_size = zt_vec2(checkbox_w, checkbox_h);
		ztVec2 box_pos, txt_size, txt_pos;

		txt_size = item->label == nullptr ? ztVec2::zero : zt_fontGetExtents(theme_data->font, item->label);
		if (zt_bitIsSet(item->behavior_flags, ztGuiCheckboxBehaviorFlags_RightText)) {
			box_pos = zt_vec2((item->size.x - box_size.x) / -2.f, 0);
			txt_pos = zt_vec2(box_pos.x + box_size.x / 2.f + padding, 0);
		}
		else {
			txt_pos = zt_vec2(item->size.x / -2.f, 0);
			box_pos = zt_vec2((item->size.x / 2.f) - (box_size.x / 2.f), 0);
		}

		zt_drawListAddText2D(draw_list, theme_data->font, item->label, zt_strLen(item->label), txt_pos + pos, ztAlign_Left, ztAnchor_Left);


		box_pos += pos;

		if (checkbox) {
			zt_drawListAddSprite(draw_list, theme_data->sprite[local::imageIndex(GuiSprite_CheckBox, GuiSprite_CheckBoxHighlight, GuiSprite_CheckBoxPressed, GuiSprite_CheckBoxDisabled, item)], zt_vec3(box_pos, 0));

			if (zt_bitIsSet(item->state_flags, zt_bit(ztGuiButtonInternalStates_IsToggled))) {
				zt_drawListAddSprite(draw_list, theme_data->sprite[GuiSprite_IconCheck], zt_vec3(box_pos, 0));
			}
		}
		else if (radio) {
			zt_drawListAddSprite(draw_list, theme_data->sprite[local::imageIndex(GuiSprite_Radio, GuiSprite_RadioHighlight, GuiSprite_RadioPressed, GuiSprite_RadioDisabled, item)], zt_vec3(box_pos, 0));

			if (zt_bitIsSet(item->state_flags, zt_bit(ztGuiButtonInternalStates_IsToggled))) {
				zt_drawListAddSprite(draw_list, theme_data->sprite[GuiSprite_RadioSelect], zt_vec3(box_pos, 0));
			}
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_TEXTEDIT_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:TextEdit");

		zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_TextEdit, -1, -1, GuiSpriteNineSlice_TextEditDisabled, item)], pos, item->size);

		ztGuiTextEditState *textedit_state = (ztGuiTextEditState*)item->state;

		ztVec2 text_pos = zt_vec2(textedit_state->text_pos[0], textedit_state->text_pos[1] + (-4 / zt_pixelsPerUnit()));

		if (textedit_state->select_beg != textedit_state->select_end) {
			int sel_beg = zt_min(textedit_state->select_beg, textedit_state->select_end);
			int sel_end = zt_max(textedit_state->select_beg, textedit_state->select_end);

			while (sel_beg < sel_end) {
				ztVec2 pos_beg = zt_guiTextEditGetCharacterPos(item, sel_beg, false);

				int idx_end_line = zt_strFindPos(textedit_state->text_buffer, "\n", sel_beg);
				if (idx_end_line == ztStrPosNotFound || idx_end_line > sel_end) {
					idx_end_line = sel_end;
				}

				ztVec2 pos_end = zt_guiTextEditGetCharacterPos(item, idx_end_line, true);

				ztVec2 pos_size = zt_vec2(pos_end.x - pos_beg.x, pos_beg.y - pos_end.y);
				ztVec2 pos_center = zt_vec2(pos_beg.x + pos_size.x / 2.f, pos_beg.y - pos_size.y / 2.f);

				zt_drawListPushColor(draw_list, ZT_GUI_HIGHLIGHT_COLOR);
				zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid], text_pos + pos_center, pos_size);
				zt_drawListPopColor(draw_list);

				sel_beg = idx_end_line + 1;
			}
		}

		ztVec3 dlpos = zt_vec3(text_pos, 0);
		zt_alignToPixel(&dlpos, zt_pixelsPerUnit());
		zt_drawListAddDrawList(draw_list, item->draw_list, dlpos);

		if (item->gm->focus_item == item) {
			if (textedit_state->cursor_vis) {
				ztFontID font = theme_data->font;
				ztVec2 cursor_pos = text_pos + zt_vec2(textedit_state->cursor_xy[0], textedit_state->cursor_xy[1]);
				ztVec2 cursor_size = zt_fontGetExtents(font, "|");
				cursor_pos.x -= cursor_size.x / 2;
				zt_drawListAddText2D(draw_list, font, "|", cursor_pos, ztAlign_Left | ztAlign_Top, ztAnchor_Left | ztAnchor_Top);
			}
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_SLIDER_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:Slider");
		ztVec2 handle_pos, handle_size;

		ztGuiSliderState * slider_state = (ztGuiSliderState*)item->state;

		if (slider_state->drag_state.dragging) {
			highlighted = true;
		}

		r32 handle_w = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_SliderHandleSize, item);
		r32 scale = 1.f;

		if (slider_state->orient == ztGuiItemOrient_Horz) {
			handle_pos = pos + zt_vec2(slider_state->handle_pos, 0);
			handle_size = zt_vec2(zt_max(8 / ppu, slider_state->handle_size), item->size.y);

			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_SliderHorzBar, -1, -1, GuiSpriteNineSlice_SliderHorzBarDisabled, item)], pos, zt_vec2(item->size.x, 10 / ppu));
			zt_drawListAddSpriteFast(draw_list, theme_data->sprite[local::imageIndex(GuiSprite_SliderHorzHandle, GuiSprite_SliderHorzHandleHighlight, GuiSprite_SliderHorzHandlePressed, GuiSprite_SliderHorzHandleDisabled, item)], zt_vec3(handle_pos, 0), ztVec3::zero, zt_vec3(scale, scale, 1));
		}
		else {
			handle_pos = pos + zt_vec2(0, slider_state->handle_pos);
			handle_size = zt_vec2(item->size.x, zt_max(10 / ppu, slider_state->handle_size));

			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_SliderVertBar, -1, -1, GuiSpriteNineSlice_SliderHorzBarDisabled, item)], pos, zt_vec2(10 / ppu, item->size.y));
			zt_drawListAddSpriteFast(draw_list, theme_data->sprite[local::imageIndex(GuiSprite_SliderVertHandle, GuiSprite_SliderVertHandleHighlight, GuiSprite_SliderVertHandlePressed, GuiSprite_SliderVertHandleDisabled, item)], zt_vec3(handle_pos, 0), ztVec3::zero, zt_vec3(scale, scale, 1));
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_SCROLLBAR_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:ScrollbarSlider");

		ztVec2 handle_pos, handle_size;

		ztGuiSliderState *scrollbar_state = (ztGuiSliderState*)item->state;

		if (scrollbar_state->drag_state.dragging) {
			highlighted = true;
		}

		r32 scrollbar_button_w = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_ScrollbarButtonW, item);

		if (scrollbar_state->orient == ztGuiItemOrient_Horz) {
			handle_pos = pos + zt_vec2(scrollbar_state->handle_pos, 0);
			handle_size = zt_vec2(zt_max(10 / ppu, scrollbar_state->handle_size - (2 / ppu)), item->size.y - (2 / ppu));

			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_ScrollbarHorzBackground, -1, -1, GuiSpriteNineSlice_ScrollbarHorzBackgroundDisabled, item)], pos, zt_vec2(item->size.x, 10 / ppu));
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_ScrollbarHorzHandle, GuiSpriteNineSlice_ScrollbarHorzHandleHighlight, GuiSpriteNineSlice_ScrollbarHorzHandlePressed, GuiSpriteNineSlice_ScrollbarHorzHandleDisabled, item)], handle_pos, handle_size);
		}
		else {
			handle_pos = pos + zt_vec2(0, scrollbar_state->handle_pos);
			handle_size = zt_vec2(item->size.x - (2 / ppu), zt_max(10 / ppu, scrollbar_state->handle_size - (2 / ppu)));

			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_ScrollbarVertBackground, -1, -1, GuiSpriteNineSlice_ScrollbarVertBackgroundDisabled, item)], pos, zt_vec2(10 / ppu, item->size.y));
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_ScrollbarVertHandle, GuiSpriteNineSlice_ScrollbarVertHandleHighlight, GuiSpriteNineSlice_ScrollbarVertHandlePressed, GuiSpriteNineSlice_ScrollbarVertHandleDisabled, item)], handle_pos, handle_size);
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_COMBOBOX_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:ComboBox");

		zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_ComboBox, GuiSpriteNineSlice_ComboBoxHighlight, GuiSpriteNineSlice_ComboBoxPressed, GuiSpriteNineSlice_ComboBoxDisabled, item)], pos, item->size);

		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);

		r32 button_w = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_ComboboxButtonW, item);

		ztGuiComboboxState *combobox_state = (ztGuiComboboxState*)item->state;

		if (combobox_state->selected >= 0 && combobox_state->selected < combobox_state->contents_count) {
			r32 width = item->size.x;

			pos.x = (pos.x) + (width / -2.f + padding * 2.f);

			if (combobox_state->popup != nullptr) {
				ztGuiMenuState *combo_menu_state = (ztGuiMenuState*)combobox_state->popup->state;
				if (combo_menu_state->icons[combobox_state->selected]) {
					ztSprite *sprite = combo_menu_state->icons[combobox_state->selected];

					zt_drawListAddSprite(draw_list, sprite, zt_vec3(pos.x + sprite->half_size.x, pos.y, 0));

					pos.x += sprite->half_size.x * 2 + padding;
				}
			}
			zt_drawListAddFancyText2D(draw_list, theme_data->font, combobox_state->contents[combobox_state->selected], pos, ztAlign_Left, ztAnchor_Left);
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_MENUBAR_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:MenuBar");

		zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_MenuBar, -1, -1, GuiSpriteNineSlice_MenuBarDisabled, item)], pos, item->size);

		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);
		pos.x -= (item->size.x - padding * 2.f) / 2.f;
		pos.y += (item->size.y - padding * 2.f) / 2.f;

		ztVec2 icon;
		icon.x = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconX, item);
		icon.y = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconY, item);

		ztGuiMenuState *menu_state = (ztGuiMenuState*)item->state;

		zt_fiz(menu_state->item_count) {
			if (menu_state->icons[i] != nullptr) {
				icon.x = zt_max(icon.x, menu_state->icons[i]->half_size.x * 2.f + padding * 2.f);
				icon.y = zt_max(icon.y, menu_state->icons[i]->half_size.y * 2.f);
			}
		}

		zt_fiz(menu_state->item_count) {
			ztVec2 ext = zt_fontGetExtents(theme_data->font, menu_state->display[i]);

			if (icon.y > ext.y) ext.y = icon.y;
			if (!(menu_state->ids[i] == ztInvalidID && zt_strStartsWith(menu_state->display[i], "__"))) {
				if (menu_state->highlighted == i && highlighted) {
					zt_drawListPushColor(draw_list, ZT_GUI_HIGHLIGHT_COLOR);
					zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid], zt_vec2(pos.x + ext.x / 2, pos.y - ext.y / 2.f), zt_vec2(ext.x + padding * 2, ext.y + 6 / ppu));
					zt_drawListPopColor(draw_list);
				}
				zt_drawListAddText2D(draw_list, theme_data->font, menu_state->display[i], zt_vec2(pos.x + icon.x, pos.y - ext.y / 2.f), ztAlign_Left, ztAnchor_Left);

				if (menu_state->icons[i] != nullptr) {
					r32 y = zt_max(menu_state->icons[i]->half_size.y, ext.y / 2.f);
					zt_drawListAddSprite(draw_list, menu_state->icons[i], zt_vec3(pos.x + padding + menu_state->icons[i]->half_size.x, pos.y - y, 0));
				}
			}

			pos.x += icon.x + padding + ext.x + padding;
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_MENU_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:Menu");

		zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_Menu, -1, -1, -1, item)], pos, item->size);

		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);
		pos.x -= (item->size.x - padding * 2.f) / 2.f;
		pos.y += (item->size.y - padding * 2.f) / 2.f;

		ztGuiMenuState *menu_state = (ztGuiMenuState*)item->state;

		ztVec2 icon;
		icon.x = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconX, item);
		icon.y = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconY, item);

		zt_fiz(menu_state->item_count) {
			if (menu_state->icons[i] != nullptr) {
				icon.x = zt_max(icon.x, menu_state->icons[i]->half_size.x * 2.f + padding * 2.f);
				icon.y = zt_max(icon.y, menu_state->icons[i]->half_size.y * 2.f);
			}
		}

		ztVec2 size = item->size;

		if (zt_guiItemIsVisible(menu_state->scrollbar_vert)) {
			pos.y += (menu_state->full_size.y - item->size.y) * zt_guiScrollbarGetValue(menu_state->scrollbar_vert);
			size.x -= menu_state->scrollbar_vert->size.x;
		}
		if (zt_guiItemIsVisible(menu_state->scrollbar_horz)) {
			pos.x -= (menu_state->full_size.x - item->size.x) * zt_guiScrollbarGetValue(menu_state->scrollbar_horz);
		}

		ztFontID font = theme_data->font;

		zt_fiz(menu_state->item_count) {
			ztVec2 ext = zt_fontGetExtents(font, menu_state->display[i]);

			if (menu_state->ids[i] == ztInvalidID && zt_strStartsWith(menu_state->display[i], "__")) {
				ext.y = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSeparatorHeight, item);

				ztVec2 lpos = pos;
				lpos.y -= ext.y / 2.f;
				zt_drawListAddSprite(draw_list, theme_data->sprite[GuiSprite_HorzLine], zt_vec3(pos.x + item->size.x / 2 - padding, pos.y, 0), ztVec3::zero, zt_vec3(item->size.x, 1, 1));
			}
			else {
				if (menu_state->highlighted == i && highlighted) {
					zt_drawListPushColor(draw_list, ZT_GUI_HIGHLIGHT_COLOR);
					zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid], zt_vec2(pos.x + size.x / 2 - padding, pos.y - ext.y / 2.f + 1 / ppu), zt_vec2(size.x - padding * 2.f, ext.y + 4 / ppu));
					zt_drawListPopColor(draw_list);
				}
				zt_drawListAddText2D(draw_list, font, menu_state->display[i], zt_vec2(pos.x + icon.x + padding, pos.y - ext.y / 2.f), ztAlign_Left, ztAnchor_Left);

				if (menu_state->icons[i] != nullptr) {
					r32 y = zt_max(menu_state->icons[i]->half_size.y, ext.y / 2.f);
					zt_drawListAddSprite(draw_list, menu_state->icons[i], zt_vec3(pos.x + padding + menu_state->icons[i]->half_size.x, pos.y - y, 0));
				}
			}


			if (menu_state->submenus[i] != nullptr) {
				r32 icon_x = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconX, item);
				r32 icon_y = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_MenuSubmenuIconY, item);
				r32 y = zt_max(icon_y, ext.y) / 2.f;
				zt_drawListAddText2D(draw_list, font, ">", zt_vec2((item->size.x + pos.x) - (padding * 3 + icon_x / 2.f), pos.y - y + padding));
			}

			pos.y -= zt_max(icon.y, ext.y) + padding;
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_TREE_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:Tree");

		zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_PanelFrame, -1, -1, GuiSpriteNineSlice_PanelFrameDisabled, item)], pos, item->size);

		ztGuiTreeState *tree_state = (ztGuiTreeState*)item->state;

		if (tree_state->active_item != nullptr) {
			bool visible = true;
			ztGuiTreeItem *parent = tree_state->active_item->parent;
			while (parent) {
				if (!parent->expanded) {
					visible = false;
					break;
				}
				parent = parent->parent;
			}
			if (visible) {
				ztGuiItem *active = tree_state->active_item->item;
				if (active) {
					ztVec2 npos = zt_guiItemPositionLocalToScreen(active, ztVec2::zero);

					zt_drawListPushColor(draw_list, ZT_GUI_HIGHLIGHT_COLOR);
					zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid], zt_vec2(pos.x, npos.y), zt_vec2(item->size.x, active->size.y));
					zt_drawListPopColor(draw_list);
				}
			}
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_SPRITE_DISPLAY_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:sprite");

		ztGuiSpriteDisplayState *sprite_display_state = (ztGuiSpriteDisplayState*)item->state;

		if (*(ztVec4*)sprite_display_state->bgcolor != ztVec4::zero) {
			zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_PanelFrame, -1, -1, GuiSpriteNineSlice_PanelFrameDisabled, item)], pos, item->size);
		}

		if (sprite_display_state->sprite_anim_controller) {
			ztSprite *sprite = zt_spriteAnimControllerActiveSprite(sprite_display_state->sprite_anim_controller);
			if (sprite != nullptr) {
				zt_drawListAddSprite(draw_list, sprite, zt_vec3(pos, 0), ztVec3::zero, zt_vec3(sprite_display_state->scale[0], sprite_display_state->scale[1], 1));
			}
		}
		else {
			zt_drawListAddGuiThemeSprite(draw_list, sprite_display_state->sprite, pos, item->size);
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_SPINNER_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:Spinner");

		zt_drawListPushColor(draw_list, ztColor_DarkGray);
		zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid], pos, item->size);
		zt_drawListPopColor(draw_list);

		zt_drawListAddSprite(draw_list, theme_data->sprite[GuiSprite_HorzLine], zt_vec3(pos.x, pos.y, 0), ztVec3::zero, zt_vec3(item->size.x - 0 / ppu, 1, 1));

		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);
			
		ztVec3 pos_txt_p = zt_vec3(pos.x, (pos.y + (item->size.y / 2)) - (6 / ppu), 0);
		ztVec3 pos_txt_m = zt_vec3(pos.x, (pos.y - (item->size.y / 2)) + (6 / ppu), 0);

		zt_drawListAddSprite(draw_list, theme_data->sprite[GuiSprite_IconArrowUp], pos_txt_p);
		zt_drawListAddSprite(draw_list, theme_data->sprite[GuiSprite_IconArrowDown], pos_txt_m);
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_LISTBOX_GUID) {
		ZT_PROFILE_GUI("guiThemeRenderItem:ListBox");

		zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[local::imageIndex(GuiSpriteNineSlice_PanelFrame, -1, -1, GuiSpriteNineSlice_PanelFrameDisabled, item)], pos, item->size);

		r32 padding = zt_guiThemeGetRValue(theme, ztGuiThemeValue_r32_Padding, item);

		ztGuiListboxState *listbox_state = (ztGuiListboxState*)item->state;

		int item_drawn = 0;
		zt_fiz(listbox_state->item_count) {
			item_drawn += 1;

			if (!zt_guiItemIsVisible(listbox_state->items[i])) {
				if (listbox_state->hidden[i]) {
					item_drawn -= 1;
				}
				continue;
			}

			if (listbox_state->selected[i]) {
				zt_drawListPushColor(draw_list, ZT_GUI_HIGHLIGHT_COLOR);
				zt_drawListAddSpriteNineSlice(draw_list, theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid], pos + listbox_state->container->pos + zt_vec2(0, listbox_state->items[i]->pos.y), zt_vec2(listbox_state->container->size.x - 2 / ppu, listbox_state->items[i]->size.y + 2 / ppu));
				zt_drawListPopColor(draw_list);
			}
			else if (zt_bitIsSet(item->behavior_flags, ztGuiListBoxBehaviorFlags_AlternateRowColor) && item_drawn % 2 == 0) {
				zt_drawListAddSolidRect2D(draw_list, pos + listbox_state->container->pos + zt_vec2(0, listbox_state->items[i]->pos.y), zt_vec2(listbox_state->container->size.x, listbox_state->items[i]->size.y), zt_color(.5f, .5f, 1, .125f));
			}
		}
	}

	// ================================================================================================================================================================================================

	else if (item->guid == ZT_GUI_CYCLEBOX_GUID) {
		ztGuiCycleboxState *cyclebox_state = (ztGuiCycleboxState*)item->state;
		if (cyclebox_state->selected >= 0 && cyclebox_state->selected < cyclebox_state->contents_count) {
			zt_drawListAddFancyText2D(draw_list, theme_data->font, cyclebox_state->contents[cyclebox_state->selected], pos);
		}
	}

	// ================================================================================================================================================================================================

	else {
		return false; // return false to use the default (this is the default though)
	}

	return true;
}

// ------------------------------------------------------------------------------------------------

ZT_FUNC_THEME_POSTRENDER_ITEM(guiThemePostRenderItem)
{
	ZT_PROFILE_GUI("guiThemePostRenderItem");

	GuiThemeData *theme_data = (GuiThemeData*)theme->theme_data;

	return false;
}

// ================================================================================================================================================================================================

ztInternal ztGuiThemeSprite _guiThemeMakeThemeSprite(ztSprite s)
{
	ztGuiThemeSprite gts;
	gts.type = ztGuiThemeSpriteType_Sprite;
	gts.s = s;
	return gts;
}

// ================================================================================================================================================================================================

ztInternal ztGuiThemeSprite _guiThemeMakeThemeSprite(ztSpriteNineSlice sns)
{
	ztGuiThemeSprite gts;
	gts.type = ztGuiThemeSpriteType_SpriteNineSlice;
	gts.sns = sns;
	return gts;
}

// ================================================================================================================================================================================================

ZT_FUNC_THEME_RENDER_PASS_BEGIN(guiThemeRenderPassBegin)
{
}

// ================================================================================================================================================================================================

ZT_FUNC_THEME_RENDER_PASS_END(guiThemeRenderPassEnd)
{
}

// ================================================================================================================================================================================================

bool guiThemeMake(ztGuiTheme *theme, ztAssetManager *asset_manager, ztGame *game)
{
	zt_returnValOnNull(theme, false);
	zt_returnValOnNull(asset_manager, false);

	GuiThemeData *theme_data = zt_mallocStruct(GuiThemeData);

	theme_data->texture = zt_textureMake(asset_manager, zt_assetLoad(asset_manager, "textures/gui.png"), ztTextureFlags_PixelPerfect);
	if (theme_data->texture == ztInvalidID) {
		return false;
	}

	zt_spriteManagerMake(&theme_data->sprite_manager, GuiSpriteNineSlice_MAX + GuiSprite_MAX + 1);

	if (!zt_spriteManagerLoad(&theme_data->sprite_manager, asset_manager, zt_assetLoad(asset_manager, "textures/gui.spr"), theme_data->texture)) {
		return false;
	}

	theme_data->sprite_nine_slice[GuiSpriteNineSlice_WindowFrame                     ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_window_frame");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_WindowFrameDisabled             ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_window_frame_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_PanelFrame                      ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_panel_frame");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_PanelFrameDisabled              ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_panel_frame_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CollapsingPanelFrame            ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_collapsing_panel_frame");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CollapsingPanelFrameDisabled    ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_collapsing_panel_frame_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonNormal                    ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_button_normal");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonHighlight                 ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_button_highlight");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonPressed                   ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_button_pressed");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonDisabled                  ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_button_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonToggleNormal              ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_toggle_button_normal");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonToggleHighlight           ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_toggle_button_highlight");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonTogglePressed             ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_toggle_button_pressed");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonToggleDisabled            ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_toggle_button_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonToggleOn                  ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_toggle_button_on");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ButtonToggleOnDisabled          ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_toggle_button_on_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_Menu                            ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_menu");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_MenuBar                         ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_menubar");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_MenuBarDisabled                 ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_menubar_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_TextEdit                        ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_textedit");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_TextEditDisabled                ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_textedit_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarVertBackground         ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_vert_background");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarVertBackgroundDisabled ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_vert_background_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarVertHandle             ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_vert_handle");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarVertHandleHighlight    ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_vert_handle_highlight");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarVertHandlePressed      ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_vert_handle_pressed");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarVertHandleDisabled     ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_vert_handle_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarHorzBackground         ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_horz_background");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarHorzBackgroundDisabled ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_horz_background_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarHorzHandle             ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_horz_handle");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarHorzHandleHighlight    ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_horz_handle_highlight");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarHorzHandlePressed      ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_horz_handle_pressed");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ScrollbarHorzHandleDisabled     ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_scrollbar_horz_handle_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_SliderVertBar                   ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_slider_vert_bar");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_SliderVertBarDisabled           ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_slider_vert_bar_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_SliderHorzBar                   ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_slider_horz_bar");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_SliderHorzBarDisabled           ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_slider_horz_bar_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ComboBox                        ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_combobox");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ComboBoxHighlight               ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_combobox_highlight");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ComboBoxPressed                 ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_combobox_pressed");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_ComboBoxDisabled                ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_combobox_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxBody                    ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_body");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxBodyDisabled            ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_body_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxLeft                    ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_left");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxLeftHighlight           ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_left_highlight");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxLeftPressed             ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_left_pressed");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxLeftDisabled            ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_left_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxRight                   ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_right");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxRightHighlight          ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_right_highlight");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxRightPressed            ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_right_pressed");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_CycleBoxRightDisabled           ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_cyclebox_right_disabled");
	theme_data->sprite_nine_slice[GuiSpriteNineSlice_Solid                           ] = zt_spriteManagerGetSpriteNineSlice(&theme_data->sprite_manager, "gui_solid");

	zt_fize(theme_data->sprite_nine_slice) {
		if (theme_data->sprite_nine_slice[i] == nullptr) {
			zt_logCritical("Missing GUI theme sprite (1.%d)", i);
			return false;
		}
	}

	theme_data->sprite           [GuiSprite_CheckBox                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_checkbox");
	theme_data->sprite           [GuiSprite_CheckBoxHighlight                        ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_checkbox_highlight");
	theme_data->sprite           [GuiSprite_CheckBoxPressed                          ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_checkbox_pressed");
	theme_data->sprite           [GuiSprite_CheckBoxDisabled                         ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_checkbox_disabled");
	theme_data->sprite           [GuiSprite_Radio                                    ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_radio");
	theme_data->sprite           [GuiSprite_RadioHighlight                           ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_radio_highlight");
	theme_data->sprite           [GuiSprite_RadioPressed                             ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_radio_pressed");
	theme_data->sprite           [GuiSprite_RadioDisabled                            ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_radio_disabled");
	theme_data->sprite           [GuiSprite_RadioSelect                              ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_radio_select");
	theme_data->sprite           [GuiSprite_SliderHorzHandle                         ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_horz_handle");
	theme_data->sprite           [GuiSprite_SliderHorzHandleHighlight                ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_horz_handle_highlight");
	theme_data->sprite           [GuiSprite_SliderHorzHandlePressed                  ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_horz_handle_pressed");
	theme_data->sprite           [GuiSprite_SliderHorzHandleDisabled                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_horz_handle_disabled");
	theme_data->sprite           [GuiSprite_SliderVertHandle                         ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_vert_handle");
	theme_data->sprite           [GuiSprite_SliderVertHandleHighlight                ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_vert_handle_highlight");
	theme_data->sprite           [GuiSprite_SliderVertHandlePressed                  ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_vert_handle_pressed");
	theme_data->sprite           [GuiSprite_SliderVertHandleDisabled                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_slider_vert_handle_disabled");
	theme_data->sprite           [GuiSprite_Resizer                                  ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_resizer");
	theme_data->sprite           [GuiSprite_HorzLine                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_horz_line");
	theme_data->sprite           [GuiSprite_IconPlus                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_plus");
	theme_data->sprite           [GuiSprite_IconMinus                                ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_minus");
	theme_data->sprite           [GuiSprite_IconArrowLeft                            ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_arrow_left");
	theme_data->sprite           [GuiSprite_IconArrowRight                           ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_arrow_right");
	theme_data->sprite           [GuiSprite_IconArrowUp                              ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_arrow_up");
	theme_data->sprite           [GuiSprite_IconArrowDown                            ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_arrow_down");
	theme_data->sprite           [GuiSprite_IconHandGrab                             ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_hand_grab");
	theme_data->sprite           [GuiSprite_IconHandPoint                            ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_hand_point");
	theme_data->sprite           [GuiSprite_IconPencil                               ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_pencil");
	theme_data->sprite           [GuiSprite_IconTrash                                ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_trash");
	theme_data->sprite           [GuiSprite_IconUndo                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_undo");
	theme_data->sprite           [GuiSprite_IconCopy                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_copy");
	theme_data->sprite           [GuiSprite_IconCut                                  ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_cut");
	theme_data->sprite           [GuiSprite_IconNew                                  ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_new");
	theme_data->sprite           [GuiSprite_IconSave                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_save");
	theme_data->sprite           [GuiSprite_IconOpen                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_open");
	theme_data->sprite           [GuiSprite_IconWarning                              ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_warning");
	theme_data->sprite           [GuiSprite_IconMenu                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_menu");
	theme_data->sprite           [GuiSprite_IconCopyright                            ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_copyright");
	theme_data->sprite           [GuiSprite_IconCheck                                ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_check");
	theme_data->sprite           [GuiSprite_IconX                                    ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_x");
	theme_data->sprite           [GuiSprite_IconGear                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_gear");
	theme_data->sprite           [GuiSprite_IconCancel                               ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_cancel");
	theme_data->sprite           [GuiSprite_IconPlay                                 ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_play");
	theme_data->sprite           [GuiSprite_IconPause                                ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_pause");
	theme_data->sprite           [GuiSprite_IconRewind                               ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_rewind");
	theme_data->sprite           [GuiSprite_IconFastForward                          ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_fastforward");
	theme_data->sprite           [GuiSprite_IconBegin                                ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_begin");
	theme_data->sprite           [GuiSprite_IconEnd                                  ] = zt_spriteManagerGetSprite(&theme_data->sprite_manager, "gui_icon_end");

	zt_fize(theme_data->sprite_nine_slice) {
		if (theme_data->sprite_nine_slice[i] == nullptr) {
			zt_logCritical("Missing GUI theme sprite (2.%d)", i);
			return false;
		}
	}


	zt_fize(theme_data->audio) {
		theme_data->audio[i] = ztInvalidID;
	}

	{
		// default font
		const char *data = "info face=ZeroToleranceGui size=16 bold=0 italic=0 charset=unicode=stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=0,0 outline=0\ncommon lineHeight=16 base=13 scaleW=512 scaleH=64 pages=1 packed=0\npage id=0 file=\\\".\\\"\nchars count=94\nchar id=33 x=11 y=0 width=5 height=16 xoffset=0 yoffset=0 xadvance=5 page=0 chnl=15\nchar id=34 x=15 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=35 x=23 y=0 width=11 height=16 xoffset=0 yoffset=0 xadvance=11 page=0 chnl=15\nchar id=36 x=35 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=37 x=44 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=38 x=55 y=0 width=9 height=16 xoffset=0 yoffset=0 xadvance=9 page=0 chnl=15\nchar id=39 x=65 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=40 x=71 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=41 x=78 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=42 x=86 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=43 x=94 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=44 x=106 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=45 x=111 y=0 width=5 height=16 xoffset=0 yoffset=0 xadvance=5 page=0 chnl=15\nchar id=46 x=118 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=47 x=124 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=48 x=133 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=49 x=142 y=0 width=5 height=16 xoffset=0 yoffset=0 xadvance=5 page=0 chnl=15\nchar id=50 x=149 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=51 x=157 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=52 x=166 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=53 x=175 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=54 x=183 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=55 x=191 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=56 x=200 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=57 x=208 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=58 x=217 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=59 x=223 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=60 x=230 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=61 x=240 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=62 x=252 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=63 x=263 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=64 x=271 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=65 x=281 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=66 x=291 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=67 x=300 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=68 x=309 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=69 x=318 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=70 x=326 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=71 x=334 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=72 x=343 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=73 x=353 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=74 x=358 y=0 width=5 height=16 xoffset=0 yoffset=0 xadvance=5 page=0 chnl=15\nchar id=75 x=365 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=76 x=374 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=77 x=382 y=0 width=9 height=16 xoffset=0 yoffset=0 xadvance=9 page=0 chnl=15\nchar id=78 x=393 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=79 x=403 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=80 x=412 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=81 x=420 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=82 x=429 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=83 x=438 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=84 x=447 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=85 x=455 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=86 x=464 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=87 x=473 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=88 x=484 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=89 x=493 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=90 x=501 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=91 x=510 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=92 x=517 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=93 x=527 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=94 x=535 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=95 x=547 y=0 width=9 height=16 xoffset=0 yoffset=0 xadvance=9 page=0 chnl=15\nchar id=96 x=558 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=97 x=564 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=98 x=572 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=99 x=581 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=100 x=589 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=101 x=598 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=102 x=606 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=103 x=613 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=104 x=622 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=105 x=631 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=106 x=636 y=0 width=5 height=16 xoffset=0 yoffset=0 xadvance=5 page=0 chnl=15\nchar id=107 x=642 y=0 width=8 height=16 xoffset=0 yoffset=0 xadvance=8 page=0 chnl=15\nchar id=108 x=651 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=109 x=657 y=0 width=10 height=16 xoffset=0 yoffset=0 xadvance=10 page=0 chnl=15\nchar id=110 x=668 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=111 x=677 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=112 x=685 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=113 x=693 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=114 x=702 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=115 x=709 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=116 x=717 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=117 x=724 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=118 x=734 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=119 x=741 y=0 width=9 height=16 xoffset=0 yoffset=0 xadvance=9 page=0 chnl=15\nchar id=120 x=751 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=121 x=759 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=122 x=767 y=0 width=6 height=16 xoffset=0 yoffset=0 xadvance=6 page=0 chnl=15\nchar id=123 x=775 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=124 x=784 y=0 width=4 height=16 xoffset=0 yoffset=0 xadvance=4 page=0 chnl=15\nchar id=125 x=789 y=0 width=7 height=16 xoffset=0 yoffset=0 xadvance=7 page=0 chnl=15\nchar id=126 x=798 y=0 width=11 height=16 xoffset=0 yoffset=0 xadvance=11 page=0 chnl=15\n";
		theme_data->font = zt_fontMakeFromBmpFontData(data, theme_data->texture, zt_vec2i(0, 20 + 473));
	}

	{
		// monospaced default
		const char *data = "info face=ZeroToleranceDefaultMono size=16 bold=0 italic=0 charset=unicode stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=0,0 outline=0\ncommon lineHeight=16 base=13 scaleW=1024 scaleH=32 pages=1 packed=0\npage id=0 file=\\\".\\\"\nchars count=94\nchar id=33 x=8 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=34 x=16 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=35 x=24 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=36 x=32 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=37 x=40 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=38 x=48 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=39 x=56 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=40 x=64 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=41 x=72 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=42 x=80 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=43 x=88 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=44 x=96 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=45 x=104 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=46 x=112 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=47 x=120 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=48 x=128 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=49 x=136 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=50 x=144 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=51 x=152 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=52 x=160 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=53 x=168 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=54 x=176 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=55 x=184 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=56 x=192 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=57 x=200 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=58 x=208 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=59 x=216 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=60 x=224 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=61 x=232 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=62 x=240 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=63 x=248 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=64 x=256 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=65 x=264 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=66 x=272 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=67 x=280 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=68 x=288 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=69 x=296 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=70 x=304 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=71 x=312 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=72 x=320 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=73 x=328 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=74 x=336 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=75 x=344 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=76 x=352 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=77 x=360 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=78 x=368 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=79 x=376 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=80 x=384 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=81 x=392 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=82 x=400 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=83 x=408 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=84 x=416 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=85 x=424 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=86 x=432 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=87 x=440 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=88 x=448 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=89 x=456 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=90 x=464 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=91 x=472 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=92 x=480 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=93 x=488 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=94 x=496 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=95 x=504 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=96 x=512 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=97 x=520 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=98 x=528 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=99 x=536 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=100 x=544 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=101 x=552 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=102 x=560 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=103 x=568 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=104 x=576 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=105 x=584 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=106 x=592 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=107 x=600 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=108 x=608 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=109 x=616 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=110 x=624 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=111 x=632 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=112 x=640 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=113 x=648 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=114 x=656 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=115 x=664 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=116 x=672 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=117 x=680 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=118 x=688 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=119 x=696 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=120 x=704 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=121 x=712 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=122 x=720 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=123 x=728 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=124 x=736 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=125 x=744 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\nchar id=126 x=744 y=0 width=8 height=16 xoffset=0 yoffset=3 xadvance=8 page=0 chnl=15\n";
		theme_data->font_monospace = zt_fontMakeFromBmpFontData(data, theme_data->texture, zt_vec2i(0, 1 + 473));
	}


	game->gui_theme_data   = theme_data;

	theme_data->game       = game;
	theme->theme_data      = theme_data;

#if 1
	theme->get_rvalue      = ZT_FUNCTION_POINTER_TO_VAR(guiThemeGetRValue);
	theme->get_ivalue      = ZT_FUNCTION_POINTER_TO_VAR(guiThemeGetIValue);
	theme->update_item     = ZT_FUNCTION_POINTER_TO_VAR(guiThemeUpdateItem);
	theme->update_subitem  = ZT_FUNCTION_POINTER_TO_VAR(guiThemeUpdateSubitem);
	theme->size_item       = ZT_FUNCTION_POINTER_TO_VAR(guiThemeSizeItem);
	theme->render_item     = ZT_FUNCTION_POINTER_TO_VAR(guiThemeRenderItem);
	theme->postrender_item = ZT_FUNCTION_POINTER_TO_VAR(guiThemePostRenderItem);
	theme->pass_begin      = ZT_FUNCTION_POINTER_TO_VAR(guiThemeRenderPassBegin);
	theme->pass_end        = ZT_FUNCTION_POINTER_TO_VAR(guiThemeRenderPassEnd);
#else
	theme->get_rvalue      = ztInvalidID;
	theme->get_ivalue      = ztInvalidID;
	theme->update_item     = ztInvalidID;
	theme->update_subitem  = ztInvalidID;
	theme->size_item       = ztInvalidID;
	theme->render_item     = ztInvalidID;
#endif
	theme->flags |= ztGuiThemeFlags_SeparatePasses;

	return true;
}

// ================================================================================================================================================================================================

void guiThemeFree(ztGuiTheme *theme)
{
	if (theme == nullptr) {
		return;
	}

	GuiThemeData *theme_data = (GuiThemeData*)theme->theme_data;

	zt_fontFree(theme_data->font);
	zt_fontFree(theme_data->font_monospace);

	zt_fize(theme_data->audio) {
		if (theme_data->audio[i] != ztInvalidID) {
			zt_audioClipFree(theme_data->audio[i]);
			theme_data->audio[i] = ztInvalidID;
		}
	}

	zt_fize(theme_data->sprite) {
		theme_data->sprite[i] = nullptr;
	}
	zt_fize(theme_data->sprite_nine_slice) {
		theme_data->sprite_nine_slice[i] = nullptr;
	}

	zt_spriteManagerFree(&theme_data->sprite_manager);
	zt_textureFree(theme_data->texture);

	zt_fize(theme_data->audio) {
		zt_audioClipFree(theme_data->audio[i]);
	}

	zt_free(theme_data);
}

// ================================================================================================================================================================================================
