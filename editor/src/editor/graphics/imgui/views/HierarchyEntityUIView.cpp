#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "HierarchyEntityUIView.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/imgui/font_icon.h"

// utils
#include "utils/string_extensions.h"

// editor
#include "editor/core/EditorEngine.h"

// gameplay
#include "gameplay/systems/MeshSystem.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			HierarchyEntityUIView::HierarchyEntityUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID) : HierarchyUIView(a_Window), m_EntityID(a_EntityID)
			{
				gameplay::Entity* entity = core::EDITOR_ENGINE->GetECS().GetEntity(m_EntityID);
				if (!entity)
				{
					return;
				}

				if (core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::MeshSystem>().HasComponent(m_EntityID))
				{
					m_sIcon = font::ICON_CUBE;
				}
				else if (core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::SpriteSystem>().HasComponent(m_EntityID))
				{
					m_sIcon = font::ICON_IMAGE;
				}
				else if (core::EDITOR_ENGINE->GetECS().GetSystem<gameplay::TransformSystem>().HasComponent(m_EntityID))
				{
					m_sIcon = font::ICON_AXIS;
				}
				else
				{
					m_sIcon = font::ICON_CUBE;
				}
				m_sName = entity->GetName();
			}

			void HierarchyEntityUIView::RenderInHierarchy(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected)
			{
				gameplay::Entity* entity = core::EDITOR_ENGINE->GetECS().GetEntity(m_EntityID);
				if (!entity)
				{
					return;
				}

				ImVec2 screenCursorPos = ImGui::GetCursorScreenPos();

				bool wasInactive = !entity->IsActive();
				if (wasInactive)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				HierarchyUIView::RenderInHierarchy(a_bClicked, a_bDoubleClicked, a_bSelected);

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y));

				ImVec2 childSize = ImVec2(ImGui::GetContentRegionAvail().x, HIERARCHY_CHILD_SIZE);
				ImVec2 buttonCursorPos = ImVec2(ImGui::GetCursorPosX() + (m_Window.GetFramePadding().x * 3), ImGui::GetCursorPosY());
				ImVec2 initialCursorPos = ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY());

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				float checkboxHeight = ImGui::GetFrameHeight();
				ImVec2 centerPos = ImVec2(m_Window.GetWindowPadding().x, buttonCursorPos.y + (childSize.y - checkboxHeight) * 0.5f);
				ImGui::SetCursorPos(centerPos);
				bool temp = entity->IsActive();
				if (ImGui::Checkbox(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, string_extensions::StringToUpper(entity->GetName()) + "_HIERARCHY").c_str(), &temp))
				{
					entity->SetIsActive(temp);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::PopStyleVar();

				if (wasInactive)
				{
					ImGui::PopStyleVar();
				}

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE