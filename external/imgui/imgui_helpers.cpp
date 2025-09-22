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
}

#endif