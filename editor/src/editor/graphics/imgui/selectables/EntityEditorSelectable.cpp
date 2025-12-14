#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "EntityEditorSelectable.h"

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
#include "editor/graphics/imgui/RenderEditorExposable.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			EntityEditorSelectable::EntityEditorSelectable(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID) : EditorSelectable(a_Window), m_EntityID(a_EntityID)
			{
				m_sIcon = font::ICON_IMAGE;

				m_bShowRename = true;
				m_bShowDelete = true;

				m_pEntity = core::EDITOR_ENGINE->GetECS().GetEntity(m_EntityID);
				if (!m_pEntity)
				{
					return;
				}

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				for (auto* sys : core::EDITOR_ENGINE->GetECS().GetSystems())
				{
					if (sys->HasComponent(entityId))
					{
						auto* comp = sys->GetBaseComponent(entityId);

						std::string id = ImGui::IMGUI_FORMAT_ID("",
							FOLDOUT_ID, string_extensions::StringToUpper(comp->GetTypeName()) + "_INSPECTOR");

						m_aExpanded.insert(std::make_pair(id, false));
					}
				}
			}

			void EntityEditorSelectable::RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected)
			{
				gameplay::Entity* entity = core::EDITOR_ENGINE->GetECS().GetEntity(m_EntityID);
				if (!entity)
				{
					return;
				}

				bool wasInactive = !entity->IsActive();
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
				bool temp = entity->IsActive();
				if (ImGui::Checkbox(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, string_extensions::StringToUpper(entity->GetName()) + "_HIERARCHY").c_str(), &temp))
				{
					entity->SetIsActive(temp);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::PopStyleVar();

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

				ImVec2 textSize = ImGui::CalcTextSize(entity->GetName().c_str());

				// Calculate position to center the icon
				centerPos = ImVec2(
					centerPos.x + iconOffset,
					buttonCursorPos.y + (childSize.y - textSize.y) * 0.5f
				);
				ImGui::SetCursorPos(centerPos);
				ImGui::Text(entity->GetName().c_str());

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y));
				ImGui::InvisibleButton(ImGui::IMGUI_FORMAT_ID("", BUTTON_ID, string_extensions::StringToUpper(entity->GetName()) + "_HIERARCHY").c_str(), childSize);

				if (wasInactive)
				{
					ImGui::PopStyleVar();
				}

				ImGui::SetCursorScreenPos(ImVec2(screenCursorPos.x, screenCursorPos.y + childSize.y));
			}

			bool EntityEditorSelectable::RenderGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
			{
				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				bool changed = false;
				for (auto* sys : core::EDITOR_ENGINE->GetECS().GetSystems())
				{
					if (sys->HasComponent(GetEntityID()))
					{
						auto* comp = sys->GetBaseComponent(GetEntityID());

						if (RenderObjectGizmos(a_vScenePos, a_vSize, a_vPanOffset, a_fZoom, comp))
						{
							changed = true;
						}
					}
				}
				return changed;
			}

			//---------------------------------------------------------------------
			void EntityEditorSelectable::OnRename(const std::string& a_sName)
			{
				if (m_pEntity)
				{
					m_pEntity->SetName(a_sName);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
			}

			//---------------------------------------------------------------------
			void EntityEditorSelectable::OnDelete()
			{
				core::EDITOR_ENGINE->GetECS().DeleteEntity(m_EntityID);
				core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
			}

			//---------------------------------------------------------------------
			std::string EntityEditorSelectable::GetName() const
			{
				return m_pEntity ? m_pEntity->GetName() : "";
			}

			//---------------------------------------------------------------------
			std::string EntityEditorSelectable::GetIcon() const
			{
				return m_sIcon;
			}

			//---------------------------------------------------------------------
			void EntityEditorSelectable::RenderEditorFields()
			{
				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				ImGui::SetCursorPosY(0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				for (auto* sys : core::EDITOR_ENGINE->GetECS().GetSystems())
				{
					ImGui::SetCursorPosX(0);
					float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
					ImGui::SetNextItemWidth(width);
					if (sys->HasComponent(m_EntityID))
					{
						auto* comp = sys->GetBaseComponent(m_EntityID);

						ImVec2 size = m_Window.GetHeaderSize();

						float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
						width -= size.x;
						ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

						ImVec2 foldOutButtonPos = ImGui::GetCursorScreenPos();

						std::string id = ImGui::IMGUI_FORMAT_ID("",
							FOLDOUT_ID, string_extensions::StringToUpper(comp->GetTypeName()) + "_INSPECTOR");
						ImGui::FoldOutButton(
							std::string((m_aExpanded[id] ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + sys->GetSystemName() + id).c_str(), &m_aExpanded[id], ImVec2(width, size.y));
						ImGui::SameLine();
						if (ImGui::IconButton(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE, BUTTON_ID, id + "_DELETE_INSPECTOR").c_str(), size, m_Window.GetIconFont()))
						{
							sys->DeleteComponent(m_EntityID);
						}

						ImGui::PopStyleVar();
						ImGui::PopStyleVar();

						if (m_aExpanded[id])
						{
							if (RenderObjectFields(comp))
							{
								core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
							}
						}
					}
				}
				ImGui::PopStyleVar();

				ImGui::SetCursorPosX(0 + m_Window.GetFramePadding().x);
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);

				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER + std::string(" Add Component"), BUTTON_ID, "ADD_COMPONENT_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					ImVec2 buttonPos = ImGui::GetItemRectMin();

					// Set the popup window position just below the button
					ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));

					ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str());
				}

				gameplay::EntityComponentSystem& ecs = core::EDITOR_ENGINE->GetECS();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Window.GetFramePadding());
				ImGui::SetNextWindowSize(ImVec2(width, 0));
				if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str()))
				{
					for (gameplay::AbstractECSSystem* system : core::EDITOR_ENGINE->GetECS().GetSystems())
					{
						if (system->HasComponent(m_EntityID))
						{
							continue;
						}

						if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(system->GetSystemName(), MENU_ITEM_ID, "ADD_COMPONENT_MENU_INSPECTOR_" + system->GetPropertyName()).c_str()))
						{
							system->CreateBaseComponent(m_EntityID);
						}
					}

					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE