#pragma once

#ifndef IMGUI_DISABLE

#include <imgui/imgui.h>

#include <functional>
#include <stdint.h>
#include <string>
#include <map>

namespace ImGui
{
#define POPUP_WINDOW_ID "POPUP_"
#define POPUP_WINDOW_BUTTON_ID "POPUP_DIALOG_SAVE_BUTTON_"
#define CHILD_ID "CHILD_"
#define BUTTON_ID "BUTTON_"
#define COMBO_ID "COMBO_"
#define WINDOW_ID "WINDOW_"
#define MENU_ITEM_ID "MENU_ITEM_"
#define TREE_NODE_ID "TREE_NODE_"
#define MENU_ID "MENU_"
#define INPUT_ID "INPUT_"
#define CHECKBOX_ID "CHECKBOX_"
#define FOLD_ID "CHECKBOX_"
#define SLIDER_ID "SLIDER_"
#define DRAG_ITEM_ID "DRAG_ITEM_"
#define SELECTABLE_ID "SELECTABLE_"
#define FOLDOUT_ID "FOLDOUT_"
#define COLOR_WHEEL_ID "COLOR_WHEEL_"
#define TABLE_ID "TABLE_"
#define PLOT_ID "PLOT_"

	bool CheckboxButton(const std::string& a_Label, bool* a_pValue, const std::string& a_sDescription, const ImVec2& a_Size = ImVec2(0, 0), const ImVec4& a_Color = ImVec4(1, 1, 1, 1));
	bool TextButton(const std::string& a_Label, const std::string& a_sDescription, const ImVec2& a_Size = ImVec2(0, 0), const ImVec4& a_Color = ImVec4(1, 1, 1, 1));

	bool IconButton(const std::string& a_Label, const std::string& a_sDescription, const ImVec2& a_Size, const ImVec4& a_Color = ImGui::GetStyle().Colors[ImGuiCol_Text]);
	bool IconCheckboxButton(const std::string& a_Label, bool* a_pValue, const std::string& a_sDescription, const ImVec2& a_vSize, const ImVec4& a_Color = ImGui::GetStyle().Colors[ImGuiCol_Text]);
	bool ConsoleButton(const std::string& a_Label, bool* a_pValue, const std::string& a_sDescription, const ImVec2& a_vSize, int a_iNumber, const ImVec4& a_Color = ImGui::GetStyle().Colors[ImGuiCol_Text], const ImVec4& a_vCircleColor = ImGui::GetStyle().Colors[ImGuiCol_TabActive]);

	void DisplayHeader(ImFont* a_BoldFont, const std::string& a_Label);

	bool InputTextStdString(const std::string& label, std::string* str, ImGuiInputTextFlags flags = 0);

	/// <summary>
	/// Converts colors ranging 0-255 to 0-1.
	/// </summary>
	/// <param name="a_R">Red.</param>
	/// <param name="a_G">Green.</param>
	/// <param name="a_B">Blue.</param>
	/// <param name="a_A">Alpha.</param>
	/// <returns></returns>
	inline ImVec4 ConvertColorsRgba(float a_R, float a_G, float a_B, float a_A)
	{
		return ImVec4(a_R / 255.0f, a_G / 255.0f, a_B / 255.0f, a_A / 255.0f);
	}

	/// <summary>
	/// Formats a string to combine a base text, an identifier, and a name, which can be used 
	/// for ImGui widgets to ensure unique IDs and descriptive labels.
	/// </summary>
	/// <param name="a_sText">The base text to display.</param>
	/// <param name="a_sID">A unique identifier for the widget, typically used for ImGui ID scoping.</param>
	/// <param name="a_sIDName">An additional name to append, adding context or detail.</param>
	/// <returns>
	/// A formatted string combining the base text, ID, and name in a manner suitable for ImGui.
	/// </returns>
	std::string IMGUI_FORMAT_ID(const std::string& a_Text, const std::string& a_ID, const std::string& a_IDName);

	bool FoldOutButton(const std::string& a_ID, bool* a_pValue, const ImVec2& a_Size = ImVec2(0, 0));
	bool OnOffButton(
		char const* label,
		bool* p_value,
		ImVec2 const& size);

	bool StartInspectorKeyVal(const std::string& a_sId, const ImVec2& a_vFramePadding);
	bool KeyValue(std::function<void()> a_Key, std::function<bool()> a_Val);
	void EndInspectorKeyVal(const ImVec2& a_vFramePadding);

	bool VectorEdit2(const std::string& label, float col[2], float a_fSpeed = 1.0f, float a_fMin = -999999999999999, float a_fMax = 9999999999999999);
	bool VectorEdit3(const std::string& label, float col[3], float a_fSpeed = 1.0f, float a_fMin = -999999999999999, float a_fMax = 9999999999999999);
	bool IVectorEdit2(const std::string& label, int col[2]);

	void ShowTooltip(const std::string& a_sText);
}

#endif