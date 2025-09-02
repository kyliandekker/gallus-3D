#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "HierarchyWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// editor includes
#include "editor/core/EditorTool.h"
#include "editor/graphics/imgui/views/inspector/EntityInspectorView.h"

// game includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
				//---------------------------------------------------------------------
				// HierarchyWindow
				//---------------------------------------------------------------------
			HierarchyWindow::HierarchyWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_LIST) + " Hierarchy", "Hierarchy"), m_SearchBar(a_Window)
			{
				m_SearchBar.Initialize("");
			}

			//---------------------------------------------------------------------
			HierarchyWindow::~HierarchyWindow()
			{}

			//---------------------------------------------------------------------
			bool HierarchyWindow::Initialize()
			{
				core::TOOL->GetECS().OnEntitiesUpdated() += std::bind(&HierarchyWindow::UpdateEntities, this);

				core::TOOL->GetECS().OnEntityComponentsUpdated() += std::bind(&HierarchyWindow::UpdateEntityComponents, this);

				core::EDITOR_TOOL->GetEditor().GetSelectable().OnChanged() += std::bind(&HierarchyWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);
				
				gameplay::GAME.GetScene().IsDirty().OnChanged() += std::bind(&HierarchyWindow::OnSceneDirty, this, std::placeholders::_1, std::placeholders::_2);

				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			bool HierarchyWindow::Destroy()
			{
				core::TOOL->GetECS().OnEntitiesUpdated() -= std::bind(&HierarchyWindow::UpdateEntities, this);

				core::TOOL->GetECS().OnEntityComponentsUpdated() -= std::bind(&HierarchyWindow::UpdateEntityComponents, this);

				core::EDITOR_TOOL->GetEditor().GetSelectable().OnChanged() -= std::bind(&HierarchyWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

				return BaseWindow::Destroy();
			}

			//---------------------------------------------------------------------
			void HierarchyWindow::Render()
			{
				if (!core::EDITOR_TOOL)
				{
					return;
				}

				std::lock_guard<std::recursive_mutex> lock(core::TOOL->GetECS().m_EntityMutex);

				// This needs to be done at the start of the frame to avoid errors.
				// We refresh the assets that show up based on the search bar and the root directory.
				if (m_bNeedsRefresh)
				{
					m_aEntities.clear();
					m_aFilteredEntities.clear();

					for (auto& entity : core::TOOL->GetECS().GetEntities())
					{
						m_aEntities.emplace_back(m_Window, entity.GetEntityID());
					}

					for (HierarchyEntityUIView& view : m_aEntities)
					{
						gameplay::Entity* entity = core::TOOL->GetECS().GetEntity(view.GetEntityID());
						if (!entity)
						{
							continue;
						}
						if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(entity->GetName()).find(m_SearchBar.GetString()) != std::string::npos)
						{
							m_aFilteredEntities.push_back(&view);
						}
					}

					if (m_PreviousEntityID.IsValid())
					{
						auto prevEntityID = m_PreviousEntityID;
						auto it = std::find_if(
							m_aEntities.begin(),
							m_aEntities.end(),
							[prevEntityID](const HierarchyEntityUIView& view)
							{
								return view.GetEntityID() == prevEntityID;
							}
						);
						if (it != m_aEntities.end())
						{
							SetSelectable(&(*it));
						}
						else
						{
							SetSelectable(nullptr);
						}
					}
					else
					{
						SetSelectable(nullptr);
					}

					m_bNeedsRefresh = false;
				}

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				float topPosY = ImGui::GetCursorPosY();

				bool spawnEntity = false;
				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_CUBE), BUTTON_ID, "SPAWN_ENTITY_HIERARCHY").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
				{
					spawnEntity = true;
				}
				ImGui::SameLine();

				bool wasDirty = gameplay::GAME.GetScene().IsDirty() && !gameplay::GAME.IsStarted() && !gameplay::GAME.GetScene().GetScenePath().empty();
				if (!wasDirty)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_SAVE), BUTTON_ID, "SAVE_HIERARCHY").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont(), ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent)))
				{
					gameplay::GAME.GetScene().SaveData();
				}

				ImVec2 endPos = ImGui::GetCursorPos();

				if (!wasDirty)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_S) && wasDirty)
				{
					gameplay::GAME.GetScene().SaveData();
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				float searchbarWidth = 200;
				float inputPadding = m_Window.GetFramePadding().x / 2;
				ImVec2 searchBarPos = ImVec2(
					ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (searchbarWidth + m_Window.GetWindowPadding().x),
					(topPosY + (toolbarSize.y / 2)) - (((inputPadding * 2) + m_Window.GetFontSize()) / 2)
				);
				ImGui::SetCursorPos(searchBarPos);
				if (m_SearchBar.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "HIERARCHY_CONSOLE").c_str(), ImVec2(searchbarWidth, toolbarSize.y), inputPadding))
				{
					m_bNeedsRefresh = true;
				}

				ImGui::SetCursorPos(endPos);

				ImGui::EndToolbar(ImVec2(0, 0));

				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x, ImGui::GetCursorPos().y + m_Window.GetFramePadding().y));
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "ENTITIES_HIERARCHY").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					for (HierarchyEntityUIView* view : m_aFilteredEntities)
					{
						if (!view)
						{
							continue;
						}

						bool
							clicked = false,
							right_clicked = false;

						view->RenderEntity(
							clicked,
							right_clicked,
							core::EDITOR_TOOL->GetEditor().GetSelectable().get() == view
						);

						if (clicked)
						{
							SetSelectable(view);
						}
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();

				if (spawnEntity)
				{
					const HierarchyEntityUIView* derivedPtr = dynamic_cast<const HierarchyEntityUIView*>(core::EDITOR_TOOL->GetEditor().GetSelectable().get());
					if (derivedPtr)
					{
						core::EDITOR_TOOL->GetEditor().SetSelectable(nullptr, nullptr);
					}

					core::TOOL->GetECS().CreateEntity(core::TOOL->GetECS().GetUniqueName("New GameObject"));
					gameplay::GAME.GetScene().SetIsDirty(true);
				}
			}

			void HierarchyWindow::UpdateEntities()
			{
				m_bNeedsRefresh = true;
			}

			void HierarchyWindow::UpdateEntityComponents()
			{
				m_bNeedsRefresh = true;
			}

			void HierarchyWindow::SetSelectable(HierarchyEntityUIView* a_EntityView)
			{
				core::EDITOR_TOOL->GetEditor().SetSelectable(a_EntityView, a_EntityView ? new EntityInspectorView(m_Window, *a_EntityView) : nullptr);
			}

			void HierarchyWindow::OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal)
			{
				if (!newVal)
				{
					return;
				}

				const HierarchyEntityUIView* derivedPtr = dynamic_cast<const HierarchyEntityUIView*>(newVal);
				if (!derivedPtr) // New selectable is NOT an entity, so we must reset the previous folder path.
				{
					m_PreviousEntityID.SetInvalid();
				}
				else // New selectable is an entity, set folder path.
				{
					m_PreviousEntityID = derivedPtr->GetEntityID();
				}
			}

			void HierarchyWindow::OnSceneDirty(const bool oldVal, const bool newVal)
			{
				std::string name = (newVal ? "*" : "") + gameplay::GAME.GetScene().GetScenePath().filename().generic_string();
				std::string title = " - (" + name + ")";

				core::EDITOR_TOOL->GetWindow().AddTitle(title);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE