#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "EntityInspectorUIView.h"

// external
#include <typeindex>
#include <unordered_map>
#include <imgui/imgui_helpers.h>

// core
#include "editor/core/EditorEngine.h"

// graphics
#include "graphics/imgui/font_icon.h"

// editor
#include "editor/graphics/imgui/views/HierarchyEntityUIView.h"
#include "editor/graphics/imgui/views/inspector/components/ComponentUIView.h"

// game
#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// EntityInspectorUIView
			//---------------------------------------------------------------------
			EntityInspectorUIView::~EntityInspectorUIView()
			{
				for (ComponentBaseUIView* view : m_aComponents)
				{
					delete view;
				}
			}

			//---------------------------------------------------------------------
			EntityInspectorUIView::EntityInspectorUIView(ImGuiWindow& a_Window, HierarchyEntityUIView& a_HierarchyEntityUIView) : InspectorUIView(a_Window), m_HierarchyEntityUIView(a_HierarchyEntityUIView)
			{
				m_bShowRename = true;
				m_bShowDelete = true;

				m_pEntity = core::EDITOR_ENGINE->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
				if (!m_pEntity)
				{
					return;
				}

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				for (auto& [type, factory] : GetComponentUIFactoryRegistry())
				{
					if (ComponentBaseUIView* view = factory(m_Window, entityId))
					{
						if (view)
						{
							m_bShowPreview = true;
						}
						m_aComponents.push_back(view);
					}
				}
			}

			//---------------------------------------------------------------------
			void EntityInspectorUIView::OnRename(const std::string& a_sName)
			{
				if (m_pEntity)
				{
					m_pEntity->SetName(a_sName);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
			}

			//---------------------------------------------------------------------
			void EntityInspectorUIView::OnDelete()
			{
				core::EDITOR_ENGINE->GetECS().DeleteEntity(m_HierarchyEntityUIView.GetEntityID());
				core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
			}

			//---------------------------------------------------------------------
			std::string EntityInspectorUIView::GetName() const
			{
				return m_pEntity ? m_pEntity->GetName() : "";
			}

			//---------------------------------------------------------------------
			std::string EntityInspectorUIView::GetIcon() const
			{
				return m_HierarchyEntityUIView.GetIcon();
			}

			//---------------------------------------------------------------------
			void EntityInspectorUIView::Render()
			{
				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				m_pEntity = core::EDITOR_ENGINE->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
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

				gameplay::EntityComponentSystem& ecs = core::EDITOR_ENGINE->GetECS();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Window.GetFramePadding());
				ImGui::SetNextWindowSize(ImVec2(width, 0));
				if (ImGui::BeginPopup(ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "ADD_COMPONENT_MENU_INSPECTOR").c_str()))
				{
					for (gameplay::AbstractECSSystem* system : core::EDITOR_ENGINE->GetECS().GetSystems())
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

			//---------------------------------------------------------------------
			void EntityInspectorUIView::RenderPreview()
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