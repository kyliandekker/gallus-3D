#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "CameraEditorSelectable.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/imgui/font_icon.h"

// utils
#include "utils/string_extensions.h"

// gameplay
#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/RenderSerializableObject.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			CameraEditorSelectable::CameraEditorSelectable(ImGuiWindow& a_Window) : EditorSelectable(a_Window)
			{
				m_sIcon = font::ICON_CAMERA;
			}

			void CameraEditorSelectable::RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected)
			{
				// Set the size of each child
				ImVec2 childSize = ImVec2(ImGui::GetContentRegionAvail().x, 32);
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

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				float checkboxHeight = ImGui::GetFrameHeight();
				ImVec2 centerPos = ImVec2(m_Window.GetWindowPadding().x, buttonCursorPos.y + (childSize.y - checkboxHeight) * 0.5f);
				ImGui::SetCursorPos(centerPos);
				ImGui::PopStyleVar();

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

				ImVec2 textSize = ImGui::CalcTextSize(GetName().c_str());

				// Calculate position to center the icon
				centerPos = ImVec2(
					centerPos.x + iconOffset,
					buttonCursorPos.y + (childSize.y - textSize.y) * 0.5f
				);
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(GetName().c_str());

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
			}

			//---------------------------------------------------------------------
			std::string CameraEditorSelectable::GetName() const
			{
				return "Camera";
			}

			//---------------------------------------------------------------------
			std::string CameraEditorSelectable::GetIcon() const
			{
				return m_sIcon.c_str();
			}

			//---------------------------------------------------------------------
			bool CameraEditorSelectable::RenderEditorFields()
			{
				bool changed = false;

				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				ImGui::SetCursorPosY(0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

				ImGui::SetCursorPosX(0);
				float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
				ImGui::SetNextItemWidth(width);
				
				ImVec2 size = m_Window.GetHeaderSize();

				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

				ImVec2 foldOutButtonPos = ImGui::GetCursorScreenPos();

				std::string id = ImGui::IMGUI_FORMAT_ID("",
					FOLDOUT_ID, string_extensions::StringToUpper(GetName()) + "_INSPECTOR");
				ImGui::FoldOutButton(
					std::string((m_bExpanded ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + GetIcon() + " " + GetName() + id).c_str(), &m_bExpanded, ImVec2(width, size.y));

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				if (m_bExpanded)
				{
					if (RenderObjectFields(&core::EDITOR_ENGINE->GetDX12().GetCamera()))
					{
						changed = true;
					}
				}

				ImGui::PopStyleVar();

				ImGui::SetCursorPosX(0 + m_Window.GetFramePadding().x);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				return changed;
			}

			//---------------------------------------------------------------------
			bool CameraEditorSelectable::RenderGizmos(const ImRect& a_SceneViewRect)
			{
				if (RenderObjectGizmos(a_SceneViewRect, &core::EDITOR_ENGINE->GetDX12().GetCamera()))
				{
					return true;
				}
				return false;
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE