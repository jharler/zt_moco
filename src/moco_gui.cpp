/**************************************************************************************************
** file: src\moco_gui.cpp
**
** Primary functionality of moco (mo)del (co)nverter
**************************************************************************************************/

// headers ========================================================================================================================================================================================

#define ZT_GAME_GUI_INTERNAL_DECLARATIONS
#include "moco_gui.h"




// variables ======================================================================================================================================================================================

struct MocoGuiToolbarData
{
	ztGame    *game;

	char       file_name[ztFileMaxPath];

	ztGuiItem *file_name_label;

	ztGuiItem *hierarchy_panel;
	ztGuiItem *transform_panel;
	ztGuiItem *textures_panel;
	ztGuiItem *bone_hierarchy_panel;
	ztGuiItem *bone_transform_panel;
	ztGuiItem *animation_panel;

	int        active_texture;
	ztGuiItem *buttons[5];
};


// private functions ==============================================================================================================================================================================

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_animComboSelected, ZT_FUNC_GUI_COMBOBOX_ITEM_SELECTED(_mocoGuiCallback_animComboSelected))
{
	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)user_data;

	if (toolbar_data->game->game_scene_main.animator) {
		zt_animControllerStartSequence(toolbar_data->game->game_scene_main.animator, toolbar_data->game->game_scene_main.animator->sequences_name_hash[selected], .125f);
		toolbar_data->game->game_scene_main.animation_active_sequence = selected;
		toolbar_data->game->game_scene_main.animation_progress = zt_animSequencePercentComplete(toolbar_data->game->game_scene_main.animator->sequences[selected]);
	}
}

// ================================================================================================================================================================================================

ztInternal void _mocoGui_createAnimWindow(MocoGuiToolbarData *toolbar_data)
{
	if (toolbar_data->game->game_scene_main.animator == nullptr) {
		return;
	}

	ztGuiItem *panel = zt_guiMakeWindow("Animations", ztGuiWindowBehaviorFlags_AllowCollapse | ztGuiWindowBehaviorFlags_AllowDrag | ztGuiWindowBehaviorFlags_AllowClose | ztGuiWindowBehaviorFlags_ShowTitle | ztGuiWindowBehaviorFlags_CloseHides);
	ztGuiItem *sizer = zt_guiMakeSizer(zt_guiWindowGetContentParent(panel), ztGuiItemOrient_Vert, true);
	zt_guiItemSetSize(panel, zt_vec2(6.5f, 1.75f));
	zt_guiItemSetPosition(panel, ztAlign_Left | ztAlign_Top, ztAnchor_Left | ztAnchor_Top, zt_vec2(27.f, -1.25f));
	//zt_guiItemSetPosition(panel, ztAlign_Left | ztAlign_Top, ztAnchor_Left | ztAnchor_Top, zt_vec2(5.f, -1.25f));

	r32 padding = 3 / zt_pixelsPerUnit();

	ztAnimController *animator = toolbar_data->game->game_scene_main.animator;

	ztGuiItem *combo = zt_guiMakeComboBox(sizer, animator->sequences_count);
	zt_fiz(animator->sequences_count) {
		zt_guiComboBoxAppend(combo, animator->sequences_name[i]);
	}
	zt_guiComboBoxSetCallback(combo, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_animComboSelected), toolbar_data);
	zt_guiSizerAddItem(sizer, combo, 0, padding);

	ztGuiItem *col_sizer = zt_guiMakeColumnSizer(sizer, 2, ztGuiColumnSizerType_FillRow);
	zt_guiSizerAddItem(sizer, col_sizer, 1, 0);
	zt_guiColumnSizerSetProp(col_sizer, 1, 1);
	
	ztGuiItem *progress_label = zt_guiMakeStaticText(sizer, "Progress:");
	ztGuiItem *progress_slider = zt_guiMakeSlider(sizer, ztGuiItemOrient_Horz, &toolbar_data->game->game_scene_main.animation_progress);
	zt_guiSizerAddItem(col_sizer, progress_label, 0, padding);
	zt_guiSizerAddItem(col_sizer, progress_slider, 1, padding);

	ztGuiItem *speed_label = zt_guiMakeStaticText(sizer, "Speed:");
	ztGuiItem *speed_slider = zt_guiMakeSlider(sizer, ztGuiItemOrient_Horz, &toolbar_data->game->game_scene_main.animation_speed);
	zt_guiSizerAddItem(col_sizer, speed_label, 0, padding);
	zt_guiSizerAddItem(col_sizer, speed_slider, 1, padding);

	zt_guiWindowCollapse(panel, true);

	toolbar_data->animation_panel = panel;
}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_loadTextureDialog, ZT_FUNC_GUI_DIALOG_FILE_SELECTED(_mocoGuiCallback_loadTextureDialog))
{
	zt_assert(zt_fileExists(path));

	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)user_data;

	ztTextureID tex_id = zt_textureMakeFromFile(path);
	if (tex_id == ztInvalidID) {
		zt_strMakePrintf(message, 4096, "Unable to load texture:\n%s", path);
		zt_guiDialogMessageBoxOk("Error Loading Texture", message, ZT_FUNCTION_POINTER_TO_VAR_NULL, nullptr);
		return;
	}

	if (toolbar_data->game->game_scene_main.active_model->type == ztModelType_Empty) {
		struct Textures
		{
			static void replace(ztModel *model, i32 idx, ztTextureID replace_with, MocoGuiToolbarData *toolbar_data)
			{
				switch (idx)
				{
					case 0: model->material.diffuse_tex = replace_with; break;
					case 1: model->material.specular_tex = replace_with; break;
					case 2: model->material.normal_tex = replace_with; break;
					case 3: model->material.height_tex = replace_with; break;
					case 4: model->material.roughness_tex = replace_with; break;
				}

				zt_flink(child, model->first_child) {
					replace(child, idx, replace_with, toolbar_data);
				}
			}
		};

		zt_guiButtonSetIcon(toolbar_data->buttons[toolbar_data->active_texture], &zt_spriteMake(tex_id, zt_vec2i(0, 0), zt_textureGetSize(tex_id)));
		toolbar_data->buttons[toolbar_data->active_texture]->button.icon->half_size = ztVec2::one;
		zt_guiItemSetSize(toolbar_data->buttons[toolbar_data->active_texture], zt_vec2(2.25f, 2.75f));

		Textures::replace(toolbar_data->game->game_scene_main.active_model, toolbar_data->active_texture, tex_id, toolbar_data);
	}
	else {
		struct Textures
		{
			static void replace(i32 idx, ztTextureID *to_replace, ztTextureID replace_with, MocoGuiToolbarData *toolbar_data)
			{
				if (*to_replace != ztInvalidID) {
					zt_textureFree(*to_replace);
					toolbar_data->buttons[idx]->button.icon->tex = replace_with;
				}
				else {
					zt_guiButtonSetIcon(toolbar_data->buttons[idx], &zt_spriteMake(replace_with, zt_vec2i(0, 0), zt_textureGetSize(replace_with)));
					toolbar_data->buttons[idx]->button.icon->half_size = ztVec2::one;
					zt_guiItemSetSize(toolbar_data->buttons[idx], zt_vec2(2.25f, 2.75f));
				}
				*to_replace = replace_with;
			}
		};

		switch (toolbar_data->active_texture)
		{
			case 0: Textures::replace(0, &toolbar_data->game->game_scene_main.active_model->material.diffuse_tex, tex_id, toolbar_data); break;
			case 1: Textures::replace(1, &toolbar_data->game->game_scene_main.active_model->material.specular_tex, tex_id, toolbar_data); break;
			case 2: Textures::replace(2, &toolbar_data->game->game_scene_main.active_model->material.normal_tex, tex_id, toolbar_data); break;
			case 3: Textures::replace(3, &toolbar_data->game->game_scene_main.active_model->material.height_tex, tex_id, toolbar_data); break;
			case 4: Textures::replace(4, &toolbar_data->game->game_scene_main.active_model->material.roughness_tex, tex_id, toolbar_data); break;
		}
	}
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_textureWindowPBRCheckbox, ZT_FUNC_GUI_BUTTON_PRESSED(_mocoGuiCallback_textureWindowPBRCheckbox))
{
	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)user_data;

	if (zt_guiCheckboxGetValue(button)) {
		toolbar_data->game->game_scene_main.active_model->shader = toolbar_data->game->game_scene_main.shader_pbr;
	}
	else {
		toolbar_data->game->game_scene_main.active_model->shader = toolbar_data->game->game_scene_main.shader_lit_shadow;
	}
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_textureWindowButton, ZT_FUNC_GUI_BUTTON_PRESSED(_mocoGuiCallback_textureWindowButton))
{
	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)user_data;

	toolbar_data->active_texture = (i32)zt_guiItemGetUserData(button);

	if (zt_inputKeysAccessState()[ztInputKeys_Control].pressed()) {
		struct Textures
		{
			static void remove(i32 idx, MocoGuiToolbarData *toolbar_data, ztTextureID *texture_id)
			{
				if (*texture_id != ztInvalidID) {
					zt_textureFree(*texture_id);
					*texture_id = ztInvalidID;
				}

				ztVec2 size = zt_guiItemGetSize(toolbar_data->buttons[idx]);
				zt_guiButtonSetIcon(toolbar_data->buttons[idx], nullptr);
				zt_guiItemSetSize(toolbar_data->buttons[idx], size);
			}
		};

		switch (toolbar_data->active_texture)
		{
			case 0: Textures::remove(0, toolbar_data, &toolbar_data->game->game_scene_main.active_model->material.diffuse_tex); break;
			case 1: Textures::remove(1, toolbar_data, &toolbar_data->game->game_scene_main.active_model->material.specular_tex); break;
			case 2: Textures::remove(2, toolbar_data, &toolbar_data->game->game_scene_main.active_model->material.normal_tex); break;
			case 3: Textures::remove(3, toolbar_data, &toolbar_data->game->game_scene_main.active_model->material.height_tex); break;
			case 4: Textures::remove(4, toolbar_data, &toolbar_data->game->game_scene_main.active_model->material.roughness_tex); break;
		}
	}
	else {

		char path[ztFileMaxPath];
		zt_fileGetFullPath(toolbar_data->file_name, path, zt_elementsOf(path));

		zt_guiDialogFileSelect("Choose Model File", ztGuiDialogFileSelectFlags_Open, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_loadTextureDialog), toolbar_data, path);
	}
}

// ================================================================================================================================================================================================

ztInternal void _mocoGui_createTextureWindow(ztGame *game, MocoGuiToolbarData *toolbar_data, ztModel *model)
{
	zt_strMakePrintf(title, 128, "%s - Textures", model->name == nullptr ? "(null)" : model->name);

	ztGuiItem *panel = zt_guiMakeWindow(title, ztGuiWindowBehaviorFlags_AllowCollapse | ztGuiWindowBehaviorFlags_AllowDrag | ztGuiWindowBehaviorFlags_AllowClose | ztGuiWindowBehaviorFlags_ShowTitle | ztGuiWindowBehaviorFlags_CloseHides);
	ztGuiItem *sizer = zt_guiMakeColumnSizer(zt_guiWindowGetContentParent(panel), 6, ztGuiColumnSizerType_FillRow, true);
	zt_guiItemSetPosition(panel, ztAlign_Left | ztAlign_Top, ztAnchor_Left | ztAnchor_Top, zt_vec2(11.6f, -1.25f));

	r32 padding = 3 / zt_pixelsPerUnit();

	ztGuiItem *panel_ops = zt_guiMakePanel(sizer);
	{
		ztGuiItem *panel_sizer = zt_guiMakeColumnSizer(panel_ops, 2, ztGuiColumnSizerType_FillRow, true);
		zt_guiColumnSizerSetProp(panel_sizer, 1, 1);

		ztGuiItem *chk_pbr_label = zt_guiMakeStaticText(sizer, "PBR Shader:");
		ztGuiItem *chk_pbr = zt_guiMakeCheckbox(sizer, "");
		zt_guiSizerAddItem(panel_sizer, chk_pbr_label, 0, padding);
		zt_guiSizerAddItem(panel_sizer, chk_pbr, 1, padding);
		zt_guiButtonSetCallback(chk_pbr, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_textureWindowPBRCheckbox), toolbar_data);

		ztGuiItem *diffuse_color_label = zt_guiMakeStaticText(sizer, "Diffuse/Albedo Color");
		ztGuiItem *diffuse_color = zt_guiMakeColorPicker(sizer, model->material.diffuse_color, ztGuiColorPickerBehaviorFlags_LiveEdit, &model->material.diffuse_color);
		zt_guiSizerAddItem(panel_sizer, diffuse_color_label, 0, padding);
		zt_guiSizerAddItem(panel_sizer, diffuse_color, 1, padding);

		ztGuiItem *shininess_label = zt_guiMakeStaticText(sizer, "Shininess");
		ztGuiItem *shininess_editor = zt_guiMakeEditor(sizer, "", &model->material.shininess, 0, 1, .1f);
		zt_guiSizerAddItem(panel_sizer, shininess_label, 0, padding);
		zt_guiSizerAddItem(panel_sizer, shininess_editor, 1, padding);

		zt_guiItemSetSize(panel_ops, zt_vec2(3, 2));
	}

	zt_guiSizerAddItem(sizer, panel_ops, 1, padding);

	struct Textures
	{
		static void addTexture(i32 idx, ztGuiItem *sizer, ztTextureID texture_id, const char *type, MocoGuiToolbarData *toolbar_data)
		{
			ztGuiItem *button = zt_guiMakeButton(sizer, type);
			zt_guiSizerAddItem(sizer, button, 1, 3 / zt_pixelsPerUnit());

			if (texture_id != ztInvalidID) {
				zt_guiButtonSetIcon(button, &zt_spriteMake(texture_id, zt_vec2i(0, 0), zt_textureGetSize(texture_id)));
				button->button.icon->half_size = ztVec2::one;
			}

			zt_guiButtonSetCallback(button, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_textureWindowButton), toolbar_data);
			zt_guiItemSetUserData(button, (void*)idx);

			toolbar_data->buttons[idx] = button;

			zt_guiItemSetSize(button, zt_vec2(2.25f, 2.75f));
		}
	};

	Textures::addTexture(0, sizer, model->material.diffuse_tex,   "Diffuse/Albedo",    toolbar_data);
	Textures::addTexture(1, sizer, model->material.specular_tex,  "Specular/Metallic", toolbar_data);
	Textures::addTexture(2, sizer, model->material.normal_tex,    "Normal",            toolbar_data);
	Textures::addTexture(3, sizer, model->material.height_tex,    "Height",            toolbar_data);
	Textures::addTexture(4, sizer, model->material.roughness_tex, "Roughness",         toolbar_data);

	zt_guiItemSetSize(panel, zt_guiSizerGetMinSize(sizer) + zt_vec2(.5f, 1.f));

	//zt_guiWindowCollapse(panel, true);

	toolbar_data->textures_panel = panel;
}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_modelCheckHide, ZT_FUNC_GUI_BUTTON_PRESSED(_mocoGuiCallback_modelCheckHide))
{
	ztModel *model = (ztModel*)user_data;

	if (zt_guiCheckboxGetValue(button)) {
		model->flags |= ztModelFlags_Hidden;
	}
	else {
		zt_bitRemove(model->flags, ztModelFlags_Hidden);
	}
}

// ================================================================================================================================================================================================

ztInternal void _mocoGui_createTransformWindow(MocoGuiToolbarData *toolbar_data, ztModel *model)
{
	zt_strMakePrintf(title, 128, "%s - Transform", model->name == nullptr ? "(null)" : model->name);

	ztGuiItem *panel = zt_guiMakeWindow(title, ztGuiWindowBehaviorFlags_AllowCollapse | ztGuiWindowBehaviorFlags_AllowDrag | ztGuiWindowBehaviorFlags_AllowClose | ztGuiWindowBehaviorFlags_ShowTitle | ztGuiWindowBehaviorFlags_CloseHides);
	ztGuiItem *sizer = zt_guiMakeSizer(zt_guiWindowGetContentParent(panel), ztGuiItemOrient_Vert, true);
	zt_guiItemSetSize(panel, zt_vec2(6.5f, 4.05f));
	zt_guiItemSetPosition(panel, ztAlign_Left | ztAlign_Top, ztAnchor_Left | ztAnchor_Top, zt_vec2(5.f, -1.25f));

	r32 padding = 3 / zt_pixelsPerUnit();

	ztGuiItem *position_ed = zt_guiMakeEditor(sizer, "Position", &model->transform.position, ztVec3::min, ztVec3::max, .1f, true);
	ztGuiItem *rotation_ed = zt_guiMakeEditor(sizer, "Rotation", (ztVec4*)&model->transform.rotation, ztVec4::min, ztVec4::max, .1f, true);
	ztGuiItem *scale_ed = zt_guiMakeEditor(sizer, "Scale", &model->transform.scale, ztVec3::min, ztVec3::max, .1f, true);

	ztGuiItem *check_hide = zt_guiMakeCheckbox(sizer, "Hide Model from View");
	zt_guiCheckboxSetValue(check_hide, zt_bitIsSet(model->flags, ztModelFlags_Hidden));
	zt_guiButtonSetCallback(check_hide, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_modelCheckHide), model);

	zt_guiSizerAddItem(sizer, position_ed, 0, padding);
	zt_guiSizerAddItem(sizer, rotation_ed, 0, padding);
	zt_guiSizerAddItem(sizer, scale_ed, 0, padding);
	zt_guiSizerAddItem(sizer, check_hide, 0, padding);

	toolbar_data->transform_panel = panel;
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_boneCheckDisplayAnim, ZT_FUNC_GUI_BUTTON_PRESSED(_mocoGuiCallback_boneCheckDisplayAnim))
{
	ztBone *bone = (ztBone*)user_data;

	if (zt_guiCheckboxGetValue(button)) {
		bone->flags |= MocoGuiBoneFlags_DisplayAnim;
	}
	else {
		zt_bitRemove(bone->flags, MocoGuiBoneFlags_DisplayAnim);
	}
}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ztInternal void _mocoGui_createBoneTransformWindow(MocoGuiToolbarData *toolbar_data, ztBone *bone)
{
	zt_strMakePrintf(title, 128, "Bone - %s - Transform", bone->name == nullptr ? "(null)" : bone->name);

	ztGuiItem *panel = zt_guiMakeWindow(title, ztGuiWindowBehaviorFlags_AllowCollapse | ztGuiWindowBehaviorFlags_AllowDrag | ztGuiWindowBehaviorFlags_AllowClose | ztGuiWindowBehaviorFlags_ShowTitle | ztGuiWindowBehaviorFlags_CloseHides);
	ztGuiItem *sizer = zt_guiMakeSizer(zt_guiWindowGetContentParent(panel), ztGuiItemOrient_Vert, true);
	zt_guiItemSetSize(panel, zt_vec2(6.5f, 4.05f));
	zt_guiItemSetPosition(panel, ztAlign_Right | ztAlign_Top, ztAnchor_Right | ztAnchor_Top, zt_vec2(-5.f, -1.25f));

	r32 padding = 3 / zt_pixelsPerUnit();

	ztGuiItem *position_ed = zt_guiMakeEditor(sizer, "Position", &bone->transform.position, ztVec3::min, ztVec3::max, .1f, true);
	ztGuiItem *rotation_ed = zt_guiMakeEditor(sizer, "Rotation", (ztVec4*)&bone->transform.rotation, ztVec4::min, ztVec4::max, .1f, true);
	ztGuiItem *scale_ed = zt_guiMakeEditor(sizer, "Scale", &bone->transform.scale, ztVec3::min, ztVec3::max, .1f, true);

	ztGuiItem *check_danim = zt_guiMakeCheckbox(sizer, "Display Animation Details");
	zt_guiCheckboxSetValue(check_danim, zt_bitIsSet(bone->flags, MocoGuiBoneFlags_DisplayAnim));
	zt_guiButtonSetCallback(check_danim, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_boneCheckDisplayAnim), bone);

	zt_guiSizerAddItem(sizer, position_ed, 0, padding);
	zt_guiSizerAddItem(sizer, rotation_ed, 0, padding);
	zt_guiSizerAddItem(sizer, scale_ed, 0, padding);
	zt_guiSizerAddItem(sizer, check_danim, 0, padding);

	toolbar_data->bone_transform_panel = panel;
}


// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGui_boneHierTreeItemSelected, ZT_FUNC_GUI_TREE_ITEM_SELECTED(_mocoGui_boneHierTreeItemSelected))
{
	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)user_data;
	zt_assert(toolbar_data);
	
	if (toolbar_data->bone_transform_panel) {
		zt_guiItemFree(toolbar_data->bone_transform_panel);
		toolbar_data->bone_transform_panel = nullptr;
	}

	zt_fiz(toolbar_data->game->game_scene_main.active_model->bones_count) {
		zt_bitRemove(toolbar_data->game->game_scene_main.active_model->bones[i].flags, ztBoneFlags_DebugDrawHighlight);
	}

	ztBone *bone = (ztBone*)zt_guiTreeGetNodeUserData(tree, node_id);
	bone->flags |= ztBoneFlags_DebugDrawHighlight;
	_mocoGui_createBoneTransformWindow(toolbar_data, bone);

	toolbar_data->game->game_scene_main.model_edit_widget = zt_modelEditWidgetMake(bone);
}

// ================================================================================================================================================================================================

ztInternal void _mocoGui_createBoneHierarchyWindow(ztGame *game, MocoGuiToolbarData *toolbar_data, ztModel *model)
{
	if (toolbar_data->bone_hierarchy_panel) {
		zt_guiItemFree(toolbar_data->bone_hierarchy_panel);
	}

	if (model->bones_count <= 0) {
		return;
	}

	struct Bones
	{
		static int getCount(ztBone *bone)
		{
			int count = 1;
			zt_flink(child, bone->first_child) {
				count += getCount(child);
			}

			return count;
		}

		static void populate(ztGuiItem *tree, ztBone *bone, ztGuiTreeNodeID node_parent)
		{
			char name[128];
			zt_strCpy(name, zt_elementsOf(name), bone->name == nullptr ? "(null)" : bone->name);

			ztGuiTreeNodeID node = zt_guiTreeAppend(tree, name, bone, node_parent);

			zt_flink(child, bone->first_child) {
				populate(tree, child, node);
			}
		}
	};

	ztGuiItem *panel = zt_guiMakeWindow("Bone Hierarchy", ztGuiWindowBehaviorFlags_AllowCollapse | ztGuiWindowBehaviorFlags_AllowDrag | ztGuiWindowBehaviorFlags_AllowResize | ztGuiWindowBehaviorFlags_ShowTitle | ztGuiWindowBehaviorFlags_CloseHides);
	ztGuiItem *sizer = zt_guiMakeSizer(zt_guiWindowGetContentParent(panel), ztGuiItemOrient_Vert, true);
	zt_guiItemSetSize(panel, zt_vec2(4.5f, 5.f));
	zt_guiItemSetPosition(panel, ztAlign_Right | ztAlign_Top, ztAnchor_Right | ztAnchor_Top, zt_vec2(-.25f, -1.25f));

	r32 padding = 4 / zt_pixelsPerUnit();

	ztBone *bone = &model->bones[model->bones_root_idx];

	int model_count = Bones::getCount(bone);

	ztGuiItem *model_tree = zt_guiMakeTree(zt_guiWindowGetContentParent(panel), model_count * 10);
	zt_guiSizerAddItem(sizer, model_tree, 1, padding);
	zt_guiTreeSetCallback(model_tree, ZT_FUNCTION_POINTER_TO_VAR(_mocoGui_boneHierTreeItemSelected), toolbar_data);

	Bones::populate(model_tree, bone, ztInvalidID);

	zt_guiWindowCollapse(panel, true);

	toolbar_data->bone_hierarchy_panel = panel;
}

// ================================================================================================================================================================================================

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ztInternal void _mocoGui_makeModelActive(MocoGuiToolbarData *toolbar_data, ztModel *model)
{
	if (toolbar_data->transform_panel != nullptr) {
		zt_guiItemFree(toolbar_data->transform_panel);
		toolbar_data->transform_panel = nullptr;

		toolbar_data->game->game_scene_main.model_edit_widget = zt_modelEditWidgetMake((ztModel*)nullptr);
	}
	if (toolbar_data->textures_panel != nullptr) {
		zt_guiItemFree(toolbar_data->textures_panel);
		toolbar_data->textures_panel = nullptr;
	}
	if (toolbar_data->bone_hierarchy_panel != nullptr) {
		zt_guiItemFree(toolbar_data->bone_hierarchy_panel);
		toolbar_data->bone_hierarchy_panel = nullptr;
	}
	if (toolbar_data->bone_transform_panel != nullptr) {
		zt_guiItemFree(toolbar_data->bone_transform_panel);
		toolbar_data->bone_transform_panel = nullptr;
	}

	if (model != nullptr) {
		_mocoGui_createTransformWindow(toolbar_data, model);
		_mocoGui_createTextureWindow(toolbar_data->game, toolbar_data, model);
		_mocoGui_createBoneHierarchyWindow(toolbar_data->game, toolbar_data, model);

		if (toolbar_data->game->game_scene_main.active_model) {
			zt_bitRemove(toolbar_data->game->game_scene_main.active_model->flags, ztModelFlags_DebugDrawAABB);
		}

		toolbar_data->game->game_scene_main.active_model = model;
		toolbar_data->game->game_scene_main.active_model->flags |= ztModelFlags_DebugDrawAABB;
		toolbar_data->game->game_scene_main.model_edit_widget = zt_modelEditWidgetMake(model);
	}
}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGui_hierTreeItemSelected, ZT_FUNC_GUI_TREE_ITEM_SELECTED(_mocoGui_hierTreeItemSelected))
{
	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)user_data;
	zt_assert(toolbar_data);

	ztModel *model = (ztModel*)zt_guiTreeGetNodeUserData(tree, node_id);
	_mocoGui_makeModelActive(toolbar_data, model);
}

// ================================================================================================================================================================================================

ztInternal void _mocoGui_createHierarchyWindow(ztGame *game, MocoGuiToolbarData *toolbar_data, ztModel *model)
{
	if (toolbar_data->hierarchy_panel) {
		zt_guiItemFree(toolbar_data->hierarchy_panel);
	}

	struct Models
	{
		static int getCount(ztModel *model)
		{
			int count = 1;
			zt_flink(child, model->first_child) {
				count += getCount(child);
			}

			return count;
		}

		static void populate(ztGuiItem *tree, ztModel *model, ztGuiTreeNodeID node_parent)
		{
			char name[256];
			zt_strCpy(name, zt_elementsOf(name), model->name == nullptr ? "(null)" : model->name);

			switch (model->type) {
				case ztModelType_Empty: zt_strCat(name, zt_elementsOf(name), " [e]"); break;
				case ztModelType_Mesh: zt_strCat(name, zt_elementsOf(name), " [m]"); break;
			}

			ztGuiTreeNodeID node = zt_guiTreeAppend(tree, name, model, node_parent);

			zt_flink(child, model->first_child) {
				populate(tree, child, node);
			}
		}
	};

	ztGuiItem *panel = zt_guiMakeWindow("Model Hierarchy", ztGuiWindowBehaviorFlags_AllowCollapse | ztGuiWindowBehaviorFlags_AllowDrag | ztGuiWindowBehaviorFlags_AllowResize | ztGuiWindowBehaviorFlags_ShowTitle | ztGuiWindowBehaviorFlags_CloseHides);
	ztGuiItem *sizer = zt_guiMakeSizer(zt_guiWindowGetContentParent(panel), ztGuiItemOrient_Vert, true);
	zt_guiItemSetSize(panel, zt_vec2(4.5f, 5.f));
	zt_guiItemSetPosition(panel, ztAlign_Left | ztAlign_Top, ztAnchor_Left | ztAnchor_Top, zt_vec2(.25f, -1.25f));

	r32 padding = 4 / zt_pixelsPerUnit();

	int model_count = Models::getCount(model);

	ztGuiItem *model_tree = zt_guiMakeTree(zt_guiWindowGetContentParent(panel), model_count * 10);
	zt_guiSizerAddItem(sizer, model_tree, 1, padding);
	zt_guiTreeSetCallback(model_tree, ZT_FUNCTION_POINTER_TO_VAR(_mocoGui_hierTreeItemSelected), toolbar_data);

	Models::populate(model_tree, model, ztInvalidID);

	zt_guiWindowCollapse(panel, true);

	toolbar_data->hierarchy_panel = panel;
}

// ================================================================================================================================================================================================

// ================================================================================================================================================================================================
// FILE LOADING
// ================================================================================================================================================================================================

ztInternal const char *_mocoGui_getTempFileName(ztGame *game)
{
	static char file_name[ztFileMaxPath];
	zt_fileConcatFileToPath(file_name, ztFileMaxPath, game->details->data_path, "temp.model");
	return file_name;
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_exportFileDialog, ZT_FUNC_GUI_DIALOG_FILE_SELECTED(_mocoGuiCallback_exportFileDialog))
{
	zt_assert(zt_fileExists(path));

	ztGame *game = (ztGame*)user_data;
	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)zt_guiItemGetUserData(game->toolbar);

	MocoConvertOptions options = {};
	if (game->game_scene_main.root_model) {
		options.root_transform = game->game_scene_main.root_model->calculated_mat;
	}

	zt_fileGetFullPath(path, game->last_path_export, ztFileMaxPath);

	if (zt_fileExists(path)) {
		zt_fileDelete(path);
	}

	MocoErrorType_Enum error = MocoErrorType_Success;
	if (!mocoConvertFile(&options, toolbar_data->file_name, path, &error)) {
		char error_msg[1024];
		mocoGetErrorMessage(error, error_msg, zt_elementsOf(error_msg));
		zt_guiDialogMessageBoxOk("Error Converting Model", error_msg, ZT_FUNCTION_POINTER_TO_VAR_NULL, nullptr);
		return;
	}

	zt_strMakePrintf(message, ztFileMaxPath, "File exported:\n%s", path);
	zt_guiDialogMessageBoxOk("File Successfully Exported", message, ZT_FUNCTION_POINTER_TO_VAR_NULL, nullptr);
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_loadPanelButtonExport, ZT_FUNC_GUI_BUTTON_PRESSED(_mocoGuiCallback_loadPanelButtonExport))
{
	ztGame *game = (ztGame*)user_data;

	char path[ztFileMaxPath];

	if (game->last_path_export[0] == 0) {
		zt_fileConcatFileToPath(path, ztFileMaxPath, game->details->data_path, "models");
	}
	else {
		zt_strCpy(path, ztFileMaxPath, game->last_path_export);
	}


	zt_guiDialogFileSelect("Export Model File", ztGuiDialogFileSelectFlags_Save, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_exportFileDialog), game, path);
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_loadFileDialog, ZT_FUNC_GUI_DIALOG_FILE_SELECTED(_mocoGuiCallback_loadFileDialog))
{
	zt_assert(zt_fileExists(path));

	ztGame *game = (ztGame*)user_data;

	MocoErrorType_Enum error = MocoErrorType_Success;

	const char *temp_file = _mocoGui_getTempFileName(game);
	if (zt_fileExists(temp_file)) {
		zt_fileDelete(temp_file);
	}

	zt_fileGetFullPath(path, game->last_path_import, ztFileMaxPath);

	MocoConvertOptions options = {};
	if (!mocoConvertFile(&options, path, temp_file, &error)) {
		char error_msg[1024];
		mocoGetErrorMessage(error, error_msg, zt_elementsOf(error_msg));
		zt_guiDialogMessageBoxOk("Error Converting Model", error_msg, ZT_FUNCTION_POINTER_TO_VAR_NULL, nullptr);
		return;
	}

	ztModelLoaderInput input;
	zt_memSet(&input, zt_sizeof(ztModelLoaderInput), 0);

	input.models = game->game_scene_main.models;
	input.models_size = zt_elementsOf(game->game_scene_main.models);

	input.bones = game->game_scene_main.bones;
	input.bones_size = zt_elementsOf(game->game_scene_main.bones);

	if (!zt_modelMakeFromZtmFile(&input, temp_file, game->game_scene_main.shader_lit_shadow, ztModelFlags_CastsShadows)) {
		zt_guiDialogMessageBoxOk("Error Reading Model", "Error loading ZTM file", ZT_FUNCTION_POINTER_TO_VAR_NULL, nullptr);
		return;
	}

	if (game->game_scene_main.animator) {
		zt_animControllerFree(game->game_scene_main.animator);
	}
	game->game_scene_main.animator = input.animations;

	if (game->game_scene_main.root_model) {
		zt_sceneRemoveModel(game->game_scene_main.scene, game->game_scene_main.root_model);
	}

	MocoGuiToolbarData *toolbar_data = (MocoGuiToolbarData*)zt_guiItemGetUserData(game->toolbar);
	if (toolbar_data) {
		if (toolbar_data->transform_panel) {
			zt_guiItemFree(toolbar_data->transform_panel);
			toolbar_data->transform_panel = nullptr;
		}
		if (toolbar_data->textures_panel) {
			zt_guiItemFree(toolbar_data->textures_panel);
			toolbar_data->textures_panel = nullptr;
		}
		if (toolbar_data->bone_hierarchy_panel) {
			zt_guiItemFree(toolbar_data->bone_hierarchy_panel);
			toolbar_data->bone_hierarchy_panel = nullptr;
		}
		if (toolbar_data->bone_transform_panel) {
			zt_guiItemFree(toolbar_data->bone_transform_panel);
			toolbar_data->bone_transform_panel = nullptr;
		}
		if (toolbar_data->animation_panel) {
			zt_guiItemFree(toolbar_data->animation_panel);
			toolbar_data->animation_panel = nullptr;
		}
	}

	if (input.root_model) {
		//input.root_model->transform.scale = ztVec3::one;
		zt_sceneAddModel(game->game_scene_main.scene, input.root_model);
		game->game_scene_main.models_used = input.models_used;
		_mocoGui_createHierarchyWindow(game, toolbar_data, input.root_model);
		_mocoGui_createAnimWindow(toolbar_data);
	}
	game->game_scene_main.root_model = input.root_model;
	game->game_scene_main.model_edit_widget = zt_modelEditWidgetMake((ztModel*)nullptr);

	if (game->game_scene_main.animator) {
		if (game->game_scene_main.animator->sequences_count > 0) {
			zt_animControllerStartSequence(game->game_scene_main.animator, game->game_scene_main.animator->sequences_name_hash[0], 0);
			game->game_scene_main.animation_active_sequence = 0;
			game->game_scene_main.animation_progress = zt_animSequencePercentComplete(game->game_scene_main.animator->sequences[0]);
		}
		else {
			zt_animControllerFree(game->game_scene_main.animator);
			game->game_scene_main.animator = nullptr;
		}
	}

	char file_name_only[64];
	zt_fileGetFileName(path, file_name_only, zt_elementsOf(file_name_only));

	zt_guiItemSetLabel(toolbar_data->file_name_label, file_name_only);
	zt_guiItemAutoSize(toolbar_data->file_name_label);
	zt_guiSizerRecalc(game->toolbar);

	zt_strCpy(toolbar_data->file_name, zt_elementsOf(toolbar_data->file_name), path);


	if (false && input.root_model) {
		i32 verts_needed = zt_modelGetVertices(input.root_model, nullptr, 0, ztModelGetVerticesTransform_WorldTransform);
		if (verts_needed > 0) {
			ztVec3 *vertices = zt_mallocStructArray(ztVec3, verts_needed);
			i32 vertices_count = zt_modelGetVertices(input.root_model, vertices, verts_needed, ztModelGetVerticesTransform_WorldTransform);

			game->game_scene_main.vertices = vertices;
			game->game_scene_main.vertices_count = vertices_count;

			ztVec3 aabb_center, aabb_size;
			zt_modelGetAABB(input.root_model, &aabb_center, &aabb_size);
			
			ztOcTreeItemContainedTestVerticesData test_data;
			test_data.vertices_count = vertices_count;
			test_data.vertices = vertices;
			
			zt_ocTreeMake(&game->game_scene_main.octtree, 512, 16, aabb_center, aabb_size, zt_ocTreeItemContainedTestVertices, &test_data);

//			zt_free(vertices);
		}
	}
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_loadPanelButton, ZT_FUNC_GUI_BUTTON_PRESSED(_mocoGuiCallback_loadPanelButton))
{
	ztGame *game = (ztGame*)user_data;

	char path[ztFileMaxPath];

	if (game->last_path_import[0] == 0) {
		zt_fileConcatFileToPath(path, ztFileMaxPath, game->details->data_path, "models");
	}
	else {
		zt_strCpy(path, ztFileMaxPath, game->last_path_import);
	}


	zt_guiDialogFileSelect("Choose Model File", ztGuiDialogFileSelectFlags_Open, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_loadFileDialog), game, path);
}

// ================================================================================================================================================================================================

ztInternal ztGuiItem *_mocoGui_createLoadPanel(ztGame *game, ztGuiItem *parent, MocoGuiToolbarData *toolbar_data)
{
	ztGuiItem *panel = zt_guiMakePanel(parent, 0);
	ztGuiItem *sizer = zt_guiMakeSizer(panel, ztGuiItemOrient_Horz, false);
	zt_guiSizerSizeParent(sizer, true, true);

	r32 padding = 3 / zt_pixelsPerUnit();

	ztGuiItem *load_button = zt_guiMakeButton(sizer, "Load Model File");
	zt_guiButtonSetCallback(load_button, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_loadPanelButton), game);
	zt_guiSizerAddItem(sizer, load_button, 0, padding);

	ztGuiItem *export_button = zt_guiMakeButton(sizer, "Export To ZTM");
	zt_guiButtonSetCallback(export_button, ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_loadPanelButtonExport), game);
	zt_guiSizerAddItem(sizer, export_button, 0, padding);

	ztGuiItem *file_name_label = zt_guiMakeStaticText(sizer, "", ztGuiStaticTextBehaviorFlags_Fancy, 64);
	zt_guiSizerAddItem(sizer, file_name_label, 0, padding, ztAlign_Left|ztAlign_VertCenter);
	zt_guiItemSetAlign(file_name_label, ztAlign_Left | ztAlign_VertCenter);

	zt_guiSizerAddStretcher(sizer, 1);

	toolbar_data->file_name_label = file_name_label;

	return panel;
}


// ================================================================================================================================================================================================
// TOOLBAR GUI:
// ================================================================================================================================================================================================

ztInternal void _mocoGui_repositionToolbar(ztGame *game, ztGuiItem *toolbar)
{
	// make sure toolbar is positioned properly and maintains camera width
	ztVec2 cam_ext = zt_cameraOrthoGetViewportSize(&game->camera_2d);
	zt_guiItemSetSize(toolbar, zt_vec2(cam_ext.x - .25f, .7f));
}

// ================================================================================================================================================================================================

ZT_FUNCTION_POINTER_REGISTER(_mocoGuiCallback_toolbarResizePanelUpdate, ZT_FUNC_GUI_ITEM_UPDATE(_mocoGuiCallback_toolbarResizePanelUpdate))
{
	ztGame *game = (ztGame*)user_data;

	if (game->screen_updated) {
		ztGuiItem *toolbar = zt_guiItemGetTopLevelParent(item);
		_mocoGui_repositionToolbar(game, toolbar);
	}
}

// ================================================================================================================================================================================================
// ================================================================================================================================================================================================
// ================================================================================================================================================================================================


// functions ======================================================================================================================================================================================

void mocoGuiCreateToolbar(ztGame *game)
{
	ztGuiItem *toolbar = zt_guiMakeWindow("Toolbar", 0);
	game->toolbar = toolbar;
	zt_guiItemSetPosition(toolbar, ztAlign_Center | ztAlign_Top, ztAnchor_Center | ztAnchor_Top, zt_vec2(0, -.5f));

	MocoGuiToolbarData *toolbar_data = zt_mallocStructArena(MocoGuiToolbarData, toolbar->gm->arena);
	zt_guiItemSetUserData(toolbar, toolbar_data);
	toolbar_data->game = game;
	{
		zt_guiMakePanel(toolbar, 0, toolbar_data, toolbar->gm->arena); // auto cleanup

		_mocoGui_repositionToolbar(game, toolbar);
		ztGuiItem *resize_panel = zt_guiMakePanel(toolbar);
		resize_panel->functions.update = ZT_FUNCTION_POINTER_TO_VAR(_mocoGuiCallback_toolbarResizePanelUpdate);
		resize_panel->functions.user_data = game;
	}

	ztGuiItem *main_sizer = zt_guiMakeSizer(zt_guiWindowGetContentParent(toolbar), ztGuiItemOrient_Horz);

	r32 padding = 3 / zt_pixelsPerUnit();

	ztGuiItem *load_panel = _mocoGui_createLoadPanel(game, toolbar, toolbar_data);
	zt_guiSizerAddItem(main_sizer, load_panel, 0, padding);
}

// ================================================================================================================================================================================================
