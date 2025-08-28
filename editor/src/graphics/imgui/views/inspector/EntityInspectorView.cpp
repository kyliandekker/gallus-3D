#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "EntityInspectorView.h"

#include "core/Tool.h"
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/views/HierarchyEntityUIView.h"

#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/SpriteSystem.h"

#include "graphics/imgui/views/inspector/components/TransformComponentUIView.h"
#include "graphics/imgui/views/inspector/components/SpriteComponentUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			EntityInspectorView::~EntityInspectorView()
			{
				for (ComponentBaseUIView* view : m_aComponents)
				{
					delete view;
				}
			}

			EntityInspectorView::EntityInspectorView(ImGuiWindow& a_Window, HierarchyEntityUIView& a_HierarchyEntityUIView) : InspectorView(a_Window), m_HierarchyEntityUIView(a_HierarchyEntityUIView)
			{
				m_bShowRename = true;
				m_bShowDelete = true;

				m_pEntity = core::TOOL->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
				if (!m_pEntity)
				{
					return;
				}

				gameplay::EntityComponentSystem& ecs = core::TOOL->GetECS();
				gameplay::TransformSystem& transformSystem = ecs.GetSystem<gameplay::TransformSystem>();
				gameplay::SpriteSystem& spriteSystem = ecs.GetSystem<gameplay::SpriteSystem>();
				gameplay::EntityID& entityId = m_pEntity->GetEntityID();
				if (transformSystem.HasComponent(entityId))
				{
					m_aComponents.push_back(new TransformComponentUIView(m_Window, entityId, transformSystem.GetComponent(entityId), transformSystem));
				}
				if (spriteSystem.HasComponent(m_pEntity->GetEntityID()))
				{
					m_aComponents.push_back(new SpriteComponentUIView(m_Window, entityId, spriteSystem.GetComponent(entityId), spriteSystem));
				}
			}

			void EntityInspectorView::OnRename(const std::string& a_sName)
			{
				if (m_pEntity)
				{
					m_pEntity->SetName(a_sName);
				}
			}

			void EntityInspectorView::OnDelete()
			{
				core::TOOL->GetECS().DeleteEntity(m_HierarchyEntityUIView.GetEntityID());
			}

			std::string EntityInspectorView::GetName() const
			{
				return m_pEntity ? m_pEntity->GetName() : "";
			}


			std::string EntityInspectorView::GetIcon() const
			{
				return m_HierarchyEntityUIView.GetIcon();
			}

			void EntityInspectorView::Render()
			{
				if (!m_pEntity)
				{
					return;
				}

				for (ComponentBaseUIView* component : m_aComponents)
				{
					component->Render();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetWindowPadding().y);
				}

				float width = ImGui::GetContentRegionAvail().x;
				// TODO: Add these icons back. font::ICON_COMPONENT (puzzle)
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER + std::string(" Add Component"), BUTTON_ID, "ADD_COMPONENT_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					ImVec2 buttonPos = ImGui::GetItemRectMin();

					// Set the popup window position just below the button
					ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));

					ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str());
				}

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				gameplay::EntityComponentSystem& ecs = core::TOOL->GetECS();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(m_Window.GetFramePadding().x * 2, m_Window.GetFramePadding().y * 2));
				ImGui::SetNextWindowSize(ImVec2(width, 0));
				if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str()))
				{
					ImVec4 textColor = ImGui::GetStyleColorVec4(ImGuiCol_Text);
					textColor.w = 0.5f;
					ImGui::TextColored(textColor, "Add a Component"); // TODO: Add this icon back.

					for (auto system : core::TOOL->GetECS().GetSystems())
					{
						if (system->HasComponent(entityId))
						{
							continue;
						}

						if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(system->GetSystemName(), MENU_ITEM_ID, "ADD_COMPONENT_MENU_INSPECTOR_" + system->GetPropertyName()).c_str()))
						{
							system->CreateBaseComponent(entityId);
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