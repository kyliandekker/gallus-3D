#include "BaseEntityEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "imgui_system/font_icon.h"
#include "imgui_system/ImGuiSystem.h"

// utils
#include "utils/string_extensions.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	BaseEntityEditorSelectable::BaseEntityEditorSelectable(ImGuiSystem& a_System) : EditorSelectable(a_System)
	{}

	//---------------------------------------------------------------------
	void BaseEntityEditorSelectable::RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected)
	{
		bool wasInactive = !m_bIsActive;
		if (wasInactive)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		// Set the size of each child
		ImVec2 childSize = ImVec2(ImGui::GetContentRegionAvail().x, 32);
		ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();
		ImVec2 cursorPos = screenCursorPos;
		ImVec2 childEnd = ImVec2(cursorPos.x + childSize.x, cursorPos.y + childSize.y);

		if (a_bSelected)
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)));
		}

		ImVec2 buttonCursorPos = ImVec2(ImGui::GetCursorPosX() + (m_System.GetFramePadding().x * 3), ImGui::GetCursorPosY());

		ImVec2 min = ImGui::GetCursorScreenPos(); // Top-left corner
		ImVec2 max = ImVec2(min.x + childSize.x, min.y + childSize.y); // Bottom-right corner
		if (ImGui::IsMouseHoveringRect(min, max))
		{
			ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered)));
			if (ImGui::IsMouseClicked(0))
			{
				a_bClicked = true;
			}
			if (ImGui::IsMouseDoubleClicked(0))
			{
				a_bDoubleClicked = true;
			}
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		float checkboxHeight = ImGui::GetFrameHeight();
		ImVec2 centerPos = ImVec2(m_System.GetFramePadding().x, buttonCursorPos.y + (childSize.y - checkboxHeight) * 0.5f);
		ImGui::SetCursorPos(centerPos);
		if (m_bShowActivate)
		{
			bool activate = m_bIsActive;
			if (ImGui::Checkbox(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, string_extensions::StringToUpper(GetName() + "_HIERARCHY")).c_str(), &activate))
			{
				OnActivate(activate);
			}
		}
		ImGui::PopStyleVar();

		// Dynamically calculate the size of the icon
		ImVec2 iconSize = ImVec2(m_System.GetFontSize(), m_System.GetFontSize()); // Replace this with your icon size calculation.

		float iconOffset = m_System.GetIconFont()->FontSize * 2.0f;

		// Calculate offsets for centering
		float verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

		// Final position of the icon
		centerPos.y = buttonCursorPos.y + verticalOffset;

		// Dynamically calculate the size of the icon
		iconSize = ImGui::CalcTextSize(m_sIcon.c_str()); // Replace this with your icon size calculation.

		iconOffset = m_System.GetIconFont()->FontSize * 2.0f;

		// Calculate offsets for centering
		verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

		// Final position of the icon
		centerPos = ImVec2(centerPos.x + iconOffset, buttonCursorPos.y);
		centerPos.y += verticalOffset;

		// Set cursor to the calculated position and render the icon
		ImGui::SetCursorPos(centerPos);
		ImGui::Text(m_sIcon.c_str());

		ImVec2 textSize = ImGui::CalcTextSize(GetName().c_str());

		// Calculate position to center the icon
		centerPos = ImVec2(
			centerPos.x + iconOffset,
			buttonCursorPos.y + (childSize.y - textSize.y) * 0.5f
		);
		ImGui::SetCursorPos(centerPos);
		ImGui::Text(GetName().c_str());

		ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y));
		ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, string_extensions::StringToUpper(GetName()) + "_HIERARCHY").c_str(), childSize);

		if (wasInactive)
		{
			ImGui::PopStyleVar();
		}

		ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
	}

	//---------------------------------------------------------------------
	const std::string& BaseEntityEditorSelectable::GetIcon() const
	{
		return m_sIcon;
	}
}