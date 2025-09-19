#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "EntityInspectorView.h"

#include <typeindex>
#include <unordered_map>
#include <imgui/imgui_helpers.h>

// core includes
#include "core/Tool.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/graphics/imgui/views/HierarchyEntityUIView.h"
#include "editor/graphics/imgui/ComponentFactory.h"
#include "editor/graphics/imgui/views/inspector/components/ComponentUIView.h"

// game includes
#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"

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

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				for (auto& [type, factory] : g_UIFactories)
				{
					if (auto* view = factory(m_Window, entityId))
						m_aComponents.push_back(view);
				}

				for (const ComponentBaseUIView* component : m_aComponents)
				{
					if (component->ShowPreview())
					{
						m_bShowPreview = true;
					}
				}
			}

			void EntityInspectorView::OnRename(const std::string& a_sName)
			{
				if (m_pEntity)
				{
					m_pEntity->SetName(a_sName);
					gameplay::GAME->GetScene().SetIsDirty(true);
				}
			}

			void EntityInspectorView::OnDelete()
			{
				core::TOOL->GetECS().DeleteEntity(m_HierarchyEntityUIView.GetEntityID());
				gameplay::GAME->GetScene().SetIsDirty(true);
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
				std::lock_guard<std::recursive_mutex> lock(core::TOOL->GetECS().m_EntityMutex);

				m_pEntity = core::TOOL->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
				if (!m_pEntity)
				{
					return;
				}

				if (!m_aComponents.empty())
				{
					ImGui::SetCursorPosY(0);
				}
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				for (ComponentBaseUIView* component : m_aComponents)
				{
					ImGui::SetCursorPosX(0);
					float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
					ImGui::SetNextItemWidth(width);
					component->Render();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);
				}

				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER + std::string(" Add Component"), BUTTON_ID, "ADD_COMPONENT_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					ImVec2 buttonPos = ImGui::GetItemRectMin();

					// Set the popup window position just below the button
					ImGui::SetNextWindowPos(ImVec2(buttonPos.x, buttonPos.y + ImGui::GetItemRectSize().y));

					ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str());
				}
				ImGui::PopStyleVar();

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				gameplay::EntityComponentSystem& ecs = core::TOOL->GetECS();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Window.GetFramePadding());
				ImGui::SetNextWindowSize(ImVec2(width, 0));
				if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str()))
				{
					for (gameplay::AbstractECSSystem* system : core::TOOL->GetECS().GetSystems())
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

			void EntityInspectorView::RenderPreview()
			{
				ComponentBaseUIView* view = nullptr;
				for (ComponentBaseUIView* component : m_aComponents)
				{
					if (component->ShowPreview() && (!view || component->GetPreviewPriority() > view->GetPreviewPriority()))
					{
						view = component;
					}
				}

				if (view)
				{
					view->RenderPreview();
				}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE