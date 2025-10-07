#ifndef IMGUI_DISABLE

#include "imgui/imgui_helpers.h"

#include <cmath>
#include <cstdio>
#include <string>
#include <imgui/imgui_internal.h>
#include <algorithm>
#include <unordered_set>

namespace ImGui
{
	bool CheckboxButton(const char* a_Label, bool* a_pValue, const ImVec2& a_Size, const ImVec4& a_Color)
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

		bool b = TextButton(a_Label, a_Size, a_Color);
		if (b)
		{
			*a_pValue = !(*a_pValue);
		}

		ImGui::PopStyleColor(3);

		return b;
	}


	bool TextButton(const char* a_Label, const ImVec2& a_Size, const ImVec4& a_Color)
	{
		ImVec2 pos = ImGui::GetCursorScreenPos(); // Get the top-left corner of the button

		ImGui::PushStyleColor(ImGuiCol_Text, a_Color);
		bool b = ImGui::Button(a_Label, a_Size);
		ImGui::PopStyleColor();

		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		ImU32 border_color = ImGui::GetColorU32(ImGuiCol_Border); // Use ImGui border color
		float border_thickness = ImGui::GetStyle().FrameBorderSize; // Adjust thickness as needed
		draw_list->AddRect(pos, ImVec2(pos.x + a_Size.x, pos.y + a_Size.y), border_color, 0.0f, 0, border_thickness);

		return b;
	}

	bool IconButton(const char* a_Label, const ImVec2& a_Size, ImFont* a_Font, const ImVec4& a_Color)
	{
		ImGui::PushFont(a_Font);
		bool success = ImGui::TextButton(a_Label, a_Size, a_Color);
		ImGui::PopFont();
		return success;
	}

	bool IconCheckboxButton(const char* a_Label, bool* a_pValue, const ImVec2& a_Size, ImFont* a_Font, const ImVec4& a_Color)
	{
		ImGui::PushFont(a_Font);
		bool success = ImGui::CheckboxButton(a_Label, a_pValue, a_Size, a_Color);
		ImGui::PopFont();
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

	void DisplayHeader(ImFont* a_BoldFont, const char* a_Label)
	{
		ImGui::PushFont(a_BoldFont);
		ImGui::Text(a_Label);
		ImGui::PopFont();
	}

	bool InputTextStdString(const char* label, std::string* str, ImGuiInputTextFlags flags)
	{
		IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
		flags |= ImGuiInputTextFlags_CallbackResize;

		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		return ImGui::InputText(label,
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

	std::string IMGUI_FORMAT_ID(const std::string& a_Text, const char* a_ID, const std::string& a_IDName)
	{
		std::string fullId = a_Text + a_ID + a_IDName;
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
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (a_vFramePadding.y / 2));

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, a_vFramePadding);
		bool b = ImGui::BeginTable(a_sId.c_str(), 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInnerH);
		ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed, 200.0f);
		ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 1.0f);
		return b;
	}

	void KeyValue(std::function<void()> a_Key, std::function<void()> a_Val)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		a_Key();
		ImGui::TableNextColumn();
		a_Val();
	}

	void EndInspectorKeyVal(const ImVec2& a_vFramePadding)
	{
		ImGui::EndTable();
		ImGui::PopStyleVar();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (a_vFramePadding.y / 2));
	}

    bool VectorEdit2(const char* label, float col[2])
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const float square_sz = GetFrameHeight();
        const char* label_display_end = FindRenderedTextEnd(label);
        float w_full = CalcItemWidth();
        g.NextItemData.ClearFlags();

        BeginGroup();
        PushID(label);
        const bool set_current_color_edit_id = (g.ColorEditCurrentID == 0);
        if (set_current_color_edit_id)
            g.ColorEditCurrentID = window->IDStack.back();

		const int components = 2;
        const float w_button = 0.0f;
        const float w_inputs = ImMax(w_full - w_button, 1.0f);
        w_full = w_inputs + w_button;

        // Convert to the formats we need
        float f[2] = { col[0], col[1] };

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

			value_changed |= DragFloat(ids[n], &f[n], 1.0f / 255.0f, -999999999, 999999999, fmt_table_float[fmt_idx][n]);
			value_changed_as_float |= value_changed;
		}

        if (label != label_display_end )
        {
            // Position not necessarily next to last submitted button (e.g. if style.ColorButtonPosition == ImGuiDir_Left),
            // but we need to use SameLine() to setup baseline correctly. Might want to refactor SameLine() to simplify this.
            SameLine(0.0f, style.ItemInnerSpacing.x);
            window->DC.CursorPos.x = pos.x + (w_full + style.ItemInnerSpacing.x);
            TextEx(label, label_display_end);
        }

        if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
            MarkItemEdited(g.LastItemData.ID);

		PopID();
		EndGroup();

        return value_changed;
    }

    bool IVectorEdit2(const char* label, int col[2])
    {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        const ImGuiStyle& style = g.Style;
        const float square_sz = GetFrameHeight();
        const char* label_display_end = FindRenderedTextEnd(label);
        float w_full = CalcItemWidth();
        g.NextItemData.ClearFlags();

        BeginGroup();
        PushID(label);
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
            TextEx(label, label_display_end);
        }

        if (value_changed && g.LastItemData.ID != 0) // In case of ID collision, the second EndGroup() won't catch g.ActiveId
            MarkItemEdited(g.LastItemData.ID);

		PopID();
		EndGroup();

        return value_changed;
    }
}

#endif