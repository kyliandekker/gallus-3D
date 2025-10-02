#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "HierarchyUIView.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// utils
#include "utils/string_extensions.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			void HierarchyUIView::RenderInHierarchy(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected)
			{
				// Set the size of each child
				ImVec2 childSize = ImVec2(ImGui::GetContentRegionAvail().x, HIERARCHY_CHILD_SIZE);
				ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();
				ImVec2 cursorPos = screenCursorPos;
				ImVec2 childEnd = ImVec2(cursorPos.x + childSize.x, cursorPos.y + childSize.y);

				if (a_bSelected)
				{
					ImGui::GetWindowDrawList()->AddRectFilled(cursorPos, childEnd, ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive)));
				}

				ImVec2 buttonCursorPos = ImVec2(ImGui::GetCursorPosX() + (m_Window.GetFramePadding().x * 3), ImGui::GetCursorPosY());

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

				float checkboxHeight = ImGui::GetFrameHeight();
				ImVec2 centerPos = ImVec2(m_Window.GetWindowPadding().x, buttonCursorPos.y + (childSize.y - checkboxHeight) * 0.5f);

				ImGui::PushFont(m_Window.GetIconFont());

				// Dynamically calculate the size of the icon
				ImVec2 iconSize = ImVec2(m_Window.GetFontSize(), m_Window.GetFontSize()); // Replace this with your icon size calculation.

				float iconOffset = m_Window.GetIconFont()->FontSize * 2.0f;

				// Calculate offsets for centering
				float verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

				// Final position of the icon
				centerPos.y = buttonCursorPos.y + verticalOffset;

				// Dynamically calculate the size of the icon
				iconSize = ImGui::CalcTextSize(m_sIcon.c_str()); // Replace this with your icon size calculation.

				iconOffset = m_Window.GetIconFont()->FontSize * 2.0f;

				// Calculate offsets for centering
				verticalOffset = (childSize.y - iconSize.y) / 2.0f;   // Center vertically

				// Final position of the icon
				centerPos = ImVec2(centerPos.x + iconOffset, buttonCursorPos.y);
				centerPos.y += verticalOffset;

				// Set cursor to the calculated position and render the icon
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(m_sIcon.c_str());

				ImGui::PopFont();

				ImVec2 textSize = ImGui::CalcTextSize(m_sName.c_str());

				// Calculate position to center the icon
				centerPos = ImVec2(
					centerPos.x + iconOffset,
					buttonCursorPos.y + (childSize.y - textSize.y) * 0.5f
				);
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(m_sName.c_str());

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE