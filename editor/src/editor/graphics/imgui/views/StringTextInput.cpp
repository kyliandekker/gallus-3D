#include "StringTextInput.h"

// external
#include <imgui/imgui_internal.h>
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// graphics
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// StringTextInput
	//---------------------------------------------------------------------
	StringTextInput::StringTextInput(ImGuiSystem& a_System) : ImGuiUIView(a_System)
	{}

	//---------------------------------------------------------------------
	void StringTextInput::Initialize(const std::string& a_InitialValue, size_t a_BufferSize)
	{
		m_Data = core::Data(a_BufferSize);
		SetString(a_InitialValue);
	}

	//---------------------------------------------------------------------
	void StringTextInput::SetString(const std::string& a_String)
	{
		if (a_String.size() == 0)
		{
			return;
		}

		m_Data.Clear();
		strncpy_s(reinterpret_cast<char*>(m_Data.data()), m_Data.size(), a_String.c_str(), a_String.size());
	}

	//---------------------------------------------------------------------
	std::string StringTextInput::GetString()
	{
		return std::string(reinterpret_cast<char*>(m_Data.data()));
	}

	//---------------------------------------------------------------------
	bool StringTextInput::Render(const char* a_Label, ImGuiInputTextFlags flags)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16);
		bool success = ImGui::InputText(a_Label, reinterpret_cast<char*>(m_Data.data()), m_Data.size(), flags);
		ImGui::PopStyleVar();

		if (ImGui::IsItemHovered())
		{
			// Set the cursor to a text input
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}
		return success;
	}

	//---------------------------------------------------------------------
	// SearchBarInput
	//---------------------------------------------------------------------
	SearchBarInput::SearchBarInput(ImGuiSystem& a_System) : StringTextInput(a_System)
	{}

	//---------------------------------------------------------------------
	bool SearchBarInput::Render(const char* a_Label, const ImVec2& a_Size, float a_Padding)
	{
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_System.GetFontSize() * 2.0f, a_Padding));
		ImGui::SetNextItemWidth(a_Size.x);
		bool result = ImGui::InputText(a_Label, reinterpret_cast<char*>(m_Data.data()), m_Data.size());
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();

		if (ImGui::IsItemHovered())
		{
			// Set the cursor to a text input
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
		}

		// Magnifying glass icon.
		ImGui::PushFont(m_System.GetSmallIconFont());
		ImVec2 iconSize = ImGui::CalcTextSize(font::ICON_SEARCH); // Replace this with your icon size calculation.
		ImGui::PopFont();

		ImVec2 inputSize = ImGui::GetItemRectSize();

		ImVec2 iconPos = ImVec2(cursorPos.x + m_System.GetFramePadding().x, cursorPos.y + (inputSize.y - iconSize.y) / 2);

		ImGui::SetCursorScreenPos(iconPos);
		ImGui::PushFont(m_System.GetSmallIconFont());
		ImGui::Text(font::ICON_SEARCH);
		ImGui::PopFont();

		// Magnifying glass icon.
		ImGui::PushFont(m_System.GetSmallIconFont());
		iconSize = ImGui::CalcTextSize(font::ICON_CLOSE); // Replace this with your icon size calculation.
		ImGui::PopFont();

		// Cross icon.
		iconPos = ImVec2((cursorPos.x + inputSize.x) - (m_System.GetFramePadding().x + iconSize.x), cursorPos.y + (inputSize.y - iconSize.y) / 2);

		ImVec2 clearButtonMin = iconPos;
		ImVec2 clearButtonMax = ImVec2(clearButtonMin.x + iconSize.x, clearButtonMin.y + iconSize.y);

		ImVec2 mousePos = ImGui::GetMousePos();
		bool isClearButtonHovered = (mousePos.x >= clearButtonMin.x && mousePos.x <= clearButtonMax.x &&
			mousePos.y >= clearButtonMin.y && mousePos.y <= clearButtonMax.y);

		bool isClearButtonClicked = ImGui::IsMouseClicked(0) && isClearButtonHovered;

		if (isClearButtonHovered)
		{
			// Set the cursor to a text input
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		}
		if (isClearButtonClicked)
		{
			ImGui::ClearActiveID();
			m_Data[0] = '\0';
			ImGui::SetKeyboardFocusHere(-1);
			result = true;
		}

		ImGui::SetCursorScreenPos(iconPos);
		ImGui::PushFont(m_System.GetSmallIconFont());
		ImVec4 textCol = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		if (isClearButtonHovered)
		{
			textCol.w = 0.5f;
		}
		ImGui::TextColored(textCol, font::ICON_CLOSE);
		ImGui::PopFont();

		return result;
	}
}