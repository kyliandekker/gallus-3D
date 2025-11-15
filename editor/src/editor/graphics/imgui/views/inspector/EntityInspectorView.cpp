#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "EntityInspectorView.h"

#include <typeindex>
#include <unordered_map>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>
#include <imgui/imgui_internal.h>

// core includes
#include "editor/core/EditorEngine.h"
#include "editor/EditorExpose.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"

#include "graphics/dx12/Texture.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/graphics/imgui/views/HierarchyEntityUIView.h"

// game includes
#include "gameplay/Game.h"
#include "gameplay/ECSBaseSystem.h"

#include "utils/string_extensions.h"
#include "editor/graphics/imgui/RenderEditorExposable.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// EntityInspectorView
			//---------------------------------------------------------------------
			EntityInspectorView::~EntityInspectorView()
			{
			}

			//---------------------------------------------------------------------
			EntityInspectorView::EntityInspectorView(ImGuiWindow& a_Window, HierarchyEntityUIView& a_HierarchyEntityUIView) : InspectorView(a_Window), m_HierarchyEntityUIView(a_HierarchyEntityUIView)
			{
				m_bShowRename = true;
				m_bShowDelete = true;

				m_pEntity = core::EDITOR_ENGINE->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
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

			//---------------------------------------------------------------------
			void EntityInspectorView::OnRename(const std::string& a_sName)
			{
				if (m_pEntity)
				{
					m_pEntity->SetName(a_sName);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
			}

			//---------------------------------------------------------------------
			void EntityInspectorView::OnDelete()
			{
				core::EDITOR_ENGINE->GetECS().DeleteEntity(m_HierarchyEntityUIView.GetEntityID());
				core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
			}

			//---------------------------------------------------------------------
			std::string EntityInspectorView::GetName() const
			{
				return m_pEntity ? m_pEntity->GetName() : "";
			}

			//---------------------------------------------------------------------
			std::string EntityInspectorView::GetIcon() const
			{
				return m_HierarchyEntityUIView.GetIcon();
			}

			//---------------------------------------------------------------------
			void EntityInspectorView::Render()
			{
				std::lock_guard<std::recursive_mutex> lock(core::EDITOR_ENGINE->GetECS().m_EntityMutex);

				m_pEntity = core::EDITOR_ENGINE->GetECS().GetEntity(m_HierarchyEntityUIView.GetEntityID());
				if (!m_pEntity)
				{
					return;
				}

				gameplay::EntityID& entityId = m_pEntity->GetEntityID();

				ImGui::SetCursorPosY(0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				for (auto* sys : core::EDITOR_ENGINE->GetECS().GetSystems())
				{
					ImGui::SetCursorPosX(0);
					float width = ImGui::GetContentRegionAvail().x + m_Window.GetFramePadding().x;
					ImGui::SetNextItemWidth(width);
					if (sys->HasComponent(entityId))
					{
						auto* comp = sys->GetBaseComponent(entityId);

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
							sys->DeleteComponent(entityId);
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
			void EntityInspectorView::RenderPreview()
			{
				//ComponentBaseUIView* view = nullptr;
				//for (ComponentBaseUIView* component : m_aComponents)
				//{
				//	if (component->ShowPreview() && (!view || component->GetPreviewPriority() > view->GetPreviewPriority()))
				//	{
				//		view = component;
				//	}
				//}

				//if (view)
				//{
				//	view->RenderPreview();
				//}
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE