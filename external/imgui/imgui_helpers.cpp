#ifndef IMGUI_DISABLE

#include "imgui/imgui_helpers.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <imgui/imgui_internal.h>
#include <algorithm>
#include <unordered_set>
#include <set>

namespace ImGui
{
	bool CheckboxButton(const std::string& a_Label, bool* a_pValue, const std::string& a_sDescription, const ImVec2& a_Size, const ImVec4& a_Color)
	{
		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 max = ImVec2(min.x + a_Size.x, min.y + a_Size.y);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 fill_color = *a_pValue
			? ImGui::GetColorU32(ImGuiCol_ButtonActive) // active
			: ImGui::GetColorU32(ImGuiCol_Button);      // inactive

		// Draw background first
		draw_list->AddRectFilled(min, max, fill_color, 0.0f);

		// Make button background transparent
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

		bool b = TextButton(a_Label, a_sDescription, a_Size, a_Color);
		if (b)
		{
			*a_pValue = !(*a_pValue);
		}

		ImGui::PopStyleColor(3);

		return b;
	}


	bool TextButton(const std::string& a_Label, const std::string& a_sDescription, const ImVec2& a_Size, const ImVec4& a_Color)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the top-left corner of the button

		ImGui::PushStyleColor(ImGuiCol_Text, a_Color);
		bool b = ImGui::Button(a_Label.c_str(), a_Size);
		ImGui::PopStyleColor();
		ShowTooltip(a_sDescription);

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border); // Use ImGui border color
		float border_thickness = ImGui::GetStyle().FrameBorderSize; // Adjust thickness as needed
		draw_list->AddRect(pos, ImVec2(pos.x + a_Size.x, pos.y + a_Size.y), border_color, 0.0f, 0, border_thickness);

		return b;
	}

	bool IconButton(const std::string& a_Label, const std::string& a_sDescription, const ImVec2& a_Size, const ImVec4& a_Color)
	{
		bool success = ImGui::TextButton(a_Label, a_sDescription, a_Size, a_Color);
		return success;
	}

	bool IconCheckboxButton(const std::string& a_Label, bool* a_pValue, const std::string& a_sDescription, const ImVec2& a_Size, const ImVec4& a_vColor)
	{
		bool success = ImGui::CheckboxButton(a_Label, a_pValue, a_sDescription, a_Size, a_vColor);
		return success;
	}

	bool ConsoleButton(const std::string& a_Label, bool* a_pValue, const std::string& a_sDescription, const ImVec2& a_vSize, int a_iNumber, const ImVec4& a_vColor, const ImVec4& a_vCircleColor)
	{
		ImVec2 startPos = ImGui::GetCursorScreenPos();
		ImVec2 circlePos = startPos + ImVec2(a_vSize.x * 0.75f, a_vSize.y);

		ImGui::SetCursorScreenPos(startPos);
		bool success = IconCheckboxButton(a_Label, a_pValue, a_sDescription, a_vSize, a_vColor);

		ImDrawList* pDrawList = ImGui::GetForegroundDrawList();

		const float circleRadius = 12.5f;

		ImVec4 textColorV = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImVec4 circleColorV = a_vCircleColor;

		if (!(*a_pValue))
		{
			textColorV.w = 0.4f;
			circleColorV.w = 0.4f;
		}

		ImU32 textColor = ImGui::ColorConvertFloat4ToU32(textColorV);
		ImU32 circleColor = ImGui::ColorConvertFloat4ToU32(circleColorV);

		pDrawList->AddCircleFilled(circlePos, circleRadius, circleColor);

		char numberBuffer[8] = { 0 };

		if (a_iNumber > 99)
		{
			snprintf(numberBuffer, sizeof(numberBuffer), "99");
		}
		else
		{
			snprintf(numberBuffer, sizeof(numberBuffer), "%d", a_iNumber);
		}

		ImVec2 textSize = ImGui::CalcTextSize(numberBuffer);

		ImVec2 textPos;
		textPos.x = circlePos.x - (textSize.x * 0.5f);
		textPos.y = circlePos.y - (textSize.y * 0.5f);

		pDrawList->AddText(textPos, textColor, numberBuffer);

		return success;
	}

	void BeginToolbar(const ImVec2& a_Size)
	{
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 header_pos = ImGui::GetCursorScreenPos();
		ImVec2 header_size = a_Size;

		ImVec2 header_min = header_pos;
		ImVec2 header_max = ImVec2(header_pos.x + header_size.x, header_pos.y + header_size.y);

		draw_list->AddRectFilled(header_min, header_max, ImGui::GetColorU32(ImGuiCol_Button), 0);
	}

	void EndToolbar(const ImVec2& a_Padding)
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + a_Padding.y);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + a_Padding.x);
	}

	void DisplayHeader(ImFont* a_BoldFont, const std::string& a_Label)
	{
		ImGui::PushFont(a_BoldFont);
		ImGui::Text(a_Label.c_str());
		ImGui::PopFont();
	}

	bool InputTextStdString(const std::string& label, std::string* str, ImGuiInputTextFlags flags)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		flags |= ImGuiInputTextFlags_CallbackResize;

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		return ImGui::InputText(label.c_str(),
			str->data(),
			str->capacity() + 1,
			flags,
			[](ImGuiInputTextCallbackData* data)
			{
				if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
				{
					std::string* str = (std::string*) data->UserData;
					str->resize(data->BufTextLen);
					data->Buf = str->data();
				}
				return 0;
			},
			(void*) str);
	}

	std::set<std::string> m_aIds;
	std::string IMGUI_FORMAT_ID(const std::string& a_sText, const std::string& a_sID, const std::string& a_sIDName)
	{
		std::string fullId = a_sText + "###" + a_sID + a_sIDName;
		m_aIds.insert(fullId);
		return fullId;
	}

	bool FoldOutButton(const std::string& a_ID, bool* a_pValue, const ImVec2& a_Size)
	{
		// Split visible text from ImGui ID
		std::string visibleText = a_ID;
		size_t hashPos = a_ID.find("##");
		std::string internalID = a_ID;
		if (hashPos != std::string::npos)
		{
			visibleText = a_ID.substr(0, hashPos);
			internalID = a_ID.substr(hashPos); // include ## for ImGui
		}

		ImVec2 buttonPos = ImGui::GetCursorScreenPos();

		// Draw invisible button using only the internal ID
		bool clicked = ImGui::Button(internalID.c_str(), a_Size);

		// Draw border
		ImVec2 pos = ImGui::GetItemRectMin();
		ImVec2 max = ImGui::GetItemRectMax();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRect(pos, max, ImGui::GetColorU32(ImGuiCol_Border), 0.0f, 0, ImGui::GetStyle().FrameBorderSize);

		// Draw left-aligned visible text
		ImVec2 textPos = buttonPos;
		textPos.x += ImGui::GetStyle().FramePadding.x;
		textPos.y += ImGui::GetStyle().FramePadding.y;
		draw_list->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), visibleText.c_str());

		// Toggle value
		if (clicked)
		{
			*a_pValue = !(*a_pValue);
		}

		return *a_pValue;
	}

	bool OnOffButton(
		char const* label,
		bool* p_value,
		ImVec2 const& size)
	{
		bool showLabel = label[0] != '#' && label[1] != '#' && label[0] != '\0';

		ImGuiIO& io = GetIO();
		ImGuiStyle& style = GetStyle();
		ImVec2 s(size.x - 4, size.y - 4);

		float radius_outer = std::fmin(s.x, s.y) / 2.0f;
		ImVec2 pos = GetCursorScreenPos();
		pos = ImVec2(pos.x + 2, pos.y + 2);
		ImVec2 center = ImVec2(pos.x + radius_outer, pos.y + radius_outer);

		float line_height = GetTextLineHeight();
		ImDrawList* draw_list = GetWindowDrawList();

		if (s.x != 0.0f && s.y != 0.0f)
		{
			center.x = pos.x + (s.x / 2.0f);
			center.y = pos.y + (s.y / 2.0f);
			InvisibleButton(label, ImVec2(s.x, s.y + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
		}
		else
		{
			InvisibleButton(label, ImVec2(radius_outer * 2, radius_outer * 2 + (showLabel ? line_height + style.ItemInnerSpacing.y : 0)));
		}

		bool value_changed = false;
		bool is_clicked = IsItemClicked();

		draw_list->AddCircleFilled(center, radius_outer * 0.35f, GetColorU32(ImGuiCol_Text), 16);
		draw_list->AddCircleFilled(center, radius_outer * 0.3f, *p_value ? GetColorU32(ImGuiCol_Text) : GetColorU32(ImGuiCol_Button), 16);

		if (is_clicked)
		{
			*p_value = !(*p_value);
			value_changed = true;
		}

		return value_changed;
	}

	bool StartInspectorKeyVal(const std::string& a_sId, const ImVec2& a_vFramePadding)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(a_vFramePadding.x, a_vFramePadding.x / 2));
		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, a_vFramePadding);
		bool b = ImGui::BeginTable(a_sId.c_str(), 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV);
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 200.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 1.0f);
		return b;
	}

	bool KeyValue(std::function<void()> a_Key, std::function<bool()> a_Val)
	{
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		a_Key();
		ImGui::TableSetColumnIndex(1);
		return a_Val();
	}

	void EndInspectorKeyVal(const ImVec2& a_vFramePadding)
	{
		ImGui::EndTable();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	bool VectorEdit2(const std::string& label, float col[2], float a_fSpeed, float a_fMin, float a_fMax)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const float square_sz = GetFrameHeight();
		const std::string& label_display_end = FindRenderedTextEnd(label.c_str());
		float w_full = CalcItemWidth();
		g.NextItemData.ClearFlags();

		BeginGroup();
		PushID(label.c_str());
		const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
		if (set_current_color_edit_id)
			g.ColorEditCurrentID = window->IDStack.back();

		const int components = 2;
		const float w_button = 0.0f;
		const float w_inputs = ImMax(w_full - w_button, 1.0f);
		w_full = w_inputs + w_button;

		bool value_changed = false;
		bool value_changed_as_float = false;

		const ImVec2 pos = window->DC.CursorPos;
		const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
		window->DC.CursorPos.x = pos.x + inputs_offset_x;

		// RGB/HSV 0..255 Sliders
		const float w_items = w_inputs - style.ItemInnerSpacing.x * (components - 1);

		const bool hide_prefix = (IM_TRUNC(w_items / components) <= CalcTextSize("M:0.000").x);
		static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
		static const char* fmt_table_int[3][4] =
		{
			{ "%3d", "%3d" }, // Short display
			{ "X:%3d", "T:%3d" }, // Long display for RGBA
		};
		static const char* fmt_table_float[2][2] =
		{
			{ "%0.3f", "%0.3f" }, // Short display
			{ "X:%0.3f", "Y:%0.3f" }, // Long display for RGBA
		};
		const int fmt_idx = hide_prefix ? 0 : 1;

		float prev_split = 0.0f;
		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				SameLine(0, style.ItemInnerSpacing.x);
			float next_split = IM_TRUNC(w_items * (n + 1) / components);
			SetNextItemWidth(ImMax(next_split - prev_split, 1.0f));
			prev_split = next_split;

			value_changed |= DragFloat(ids[n], &col[n], a_fSpeed, a_fMin, a_fMax, fmt_table_float[fmt_idx][n]);
			value_changed_as_float |= value_changed;
		}

		if (label != label_display_end )
		{
			// Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition == ImGuiDir_Left),
			// but we need to use SameLine() to setup baseline correctly. Might want to refactor SameLine() to simplify this.
			SameLine(0.0f, style.ItemInnerSpacing.x);
			window->DC.CursorPos.x = pos.x + (w_full + style.ItemInnerSpacing.x);
			TextEx(label.c_str(), label_display_end.c_str());
		}

		PopID();
		EndGroup();

		return value_changed_as_float;
	}

	bool VectorEdit3(const std::string& label, float col[3], float a_fSpeed, float a_fMin, float a_fMax)
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const float square_sz = GetFrameHeight();
		const std::string& label_display_end = FindRenderedTextEnd(label.c_str());
		float w_full = CalcItemWidth();
		g.NextItemData.ClearFlags();

		BeginGroup();
		PushID(label.c_str());
		const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
		if (set_current_color_edit_id)
			g.ColorEditCurrentID = window->IDStack.back();

		const int components = 3;
		const float w_button = 0.0f;
		const float w_inputs = ImMax(w_full - w_button, 1.0f);
		w_full = w_inputs + w_button;

		bool value_changed = false;
		bool value_changed_as_float = false;

		const ImVec2 pos = window->DC.CursorPos;
		const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
		window->DC.CursorPos.x = pos.x + inputs_offset_x;

		// RGB/HSV 0..255 Sliders
		const float w_items = w_inputs - style.ItemInnerSpacing.x * (components - 1);

		const bool hide_prefix = (IM_TRUNC(w_items / components) <= CalcTextSize("M:0.000").x);
		static const char* ids[3] = { "##X", "##Y", "##Z" };
		static const char* fmt_table_int[3][3] =
		{
			{ "%3d", "%3d", "%3d" }, // Short display
			{ "X:%3d", "T:%3d" }, // Long display for RGBA
		};
		static const char* fmt_table_float[3][4] =
		{
			{ "%0.3f", "%0.3f", "%0.3f" }, // Short display
			{ "X:%0.3f", "Y:%0.3f", "Z:%0.3f" }, // Long display for RGBA
		};
		const int fmt_idx = hide_prefix ? 0 : 1;

		float prev_split = 0.0f;
		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				SameLine(0, style.ItemInnerSpacing.x);
			float next_split = IM_TRUNC(w_items * (n + 1) / components);
			SetNextItemWidth(ImMax(next_split - prev_split, 1.0f));
			prev_split = next_split;

			value_changed |= DragFloat(ids[n], &col[n], a_fSpeed, a_fMin, a_fMax, fmt_table_float[fmt_idx][n]);
			value_changed_as_float |= value_changed;
		}

		if (label != label_display_end )
		{
			// Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition == ImGuiDir_Left),
			// but we need to use SameLine() to setup baseline correctly. Might want to refactor SameLine() to simplify this.
			SameLine(0.0f, style.ItemInnerSpacing.x);
			window->DC.CursorPos.x = pos.x + (w_full + style.ItemInnerSpacing.x);
			TextEx(label.c_str(), label_display_end.c_str());
		}

		PopID();
		EndGroup();

		return value_changed_as_float;
	}

	bool IVectorEdit2(const std::string& label, int col[2])
	{
		ImGuiWindow* window = GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const float square_sz = GetFrameHeight();
		const std::string& label_display_end = FindRenderedTextEnd(label.c_str());
		float w_full = CalcItemWidth();
		g.NextItemData.ClearFlags();

		BeginGroup();
		PushID(label.c_str());
		const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
		if (set_current_color_edit_id)
			g.ColorEditCurrentID = window->IDStack.back();

		const int components = 2;
		const float w_button = 0.0f;
		const float w_inputs = ImMax(w_full - w_button, 1.0f);
		w_full = w_inputs + w_button;

		// Convert to the formats we need
		int f[2] = { col[0], col[1] };

		bool value_changed = false;
		bool value_changed_as_float = false;

		const ImVec2 pos = window->DC.CursorPos;
		const float inputs_offset_x = (style.ColorButtonPosition == ImGuiDir_Left) ? w_button : 0.0f;
		window->DC.CursorPos.x = pos.x + inputs_offset_x;


			// RGB/HSV 0..255 Sliders
		const float w_items = w_inputs - style.ItemInnerSpacing.x * (components - 1);

		const bool hide_prefix = (IM_TRUNC(w_items / components) <= CalcTextSize("M:0.000").x);
		static const char* ids[4] = { "##X", "##Y", "##Z", "##W" };
		static const char* fmt_table_int[3][4] =
		{
			{ "%3d", "%3d", "%3d", "%3d" }, // Short display
			{ "X:%3d", "T:%3d" }, // Long display for RGBA
		};
		static const char* fmt_table_float[3][4] =
		{
			{ "%0.3f", "%0.3f", "%0.3f", "%0.3f" }, // Short display
			{ "X:%0.3f", "Y:%0.3f" }, // Long display for RGBA
		};
		const int fmt_idx = hide_prefix ? 0 : 1;

		float prev_split = 0.0f;
		for (int n = 0; n < components; n++)
		{
			if (n > 0)
				SameLine(0, style.ItemInnerSpacing.x);
			float next_split = IM_TRUNC(w_items * (n + 1) / components);
			SetNextItemWidth(ImMax(next_split - prev_split, 1.0f));
			prev_split = next_split;

			value_changed |= DragInt(ids[n], &f[n], 1.0f / 255.0f, -999999999, 999999999, fmt_table_float[fmt_idx][n]);
			value_changed_as_float |= value_changed;
		}

		if (label != label_display_end )
		{
			// Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition == ImGuiDir_Left),
			// but we need to use SameLine() to setup baseline correctly. Might want to refactor SameLine() to simplify this.
			SameLine(0.0f, style.ItemInnerSpacing.x);
			window->DC.CursorPos.x = pos.x + (w_full + style.ItemInnerSpacing.x);
			TextEx(label.c_str(), label_display_end.c_str());
		}

		if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
			MarkItemEdited(g.LastItemData.ID);

		PopID();
		EndGroup();

		return value_changed;
	}

	void ShowTooltip(const std::string& a_sText)
	{
		if (a_sText.empty())
		{
			return;
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
			ImGui::BeginTooltip();
			ImGui::Text(a_sText.c_str());
			ImGui::EndTooltip();
			ImGui::PopStyleVar();
		}
	}

	void ShowStaticTooltip(const ImVec2& a_vPos, const std::string& a_sText)
	{
		ImVec2 textSize = ImGui::CalcTextSize(a_sText.c_str());
		ImRect textRect = {
			{ a_vPos.x, a_vPos.y },
			{ a_vPos.x + textSize.x, a_vPos.y + textSize.y },
		};

		// Draw background.
		ImU32 backgroundColor = IM_COL32(0, 0, 0, 255);
		ImU32 textColor = IM_COL32(255, 255, 255, 255);
		ImGui::GetForegroundDrawList()->AddRectFilled(
			textRect.Min,
			textRect.Max,
			backgroundColor,
			0.0f
		);
		ImGui::GetForegroundDrawList()->AddText(
			textRect.Min,
			textColor,
			a_sText.c_str()
		);
	}
}

#endif
