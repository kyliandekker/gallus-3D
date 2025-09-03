#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerWindow.h"

#include <imgui/imgui_helpers.h>

// utils includes
#include "utils/string_extensions.h"
#include "utils/file_abstractions.h"

// graphics includes
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

// editor includes
#include "editor/core/EditorTool.h"
#include "editor/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"
#include "editor/graphics/imgui/views/inspector/ExplorerFileInspectorView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			ExplorerWindow::ExplorerWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_FOLDER) + " Explorer", "Explorer"), m_SearchBar(a_Window)
			{
				m_SearchBar.Initialize("");
			}

			bool ExplorerWindow::Initialize()
			{
				core::EDITOR_TOOL->GetEditor().GetAssetDatabase().GetOnScanCompleted() += std::bind(&ExplorerWindow::OnScanCompleted, this);

				core::EDITOR_TOOL->GetEditor().GetSelectable().OnChanged() += std::bind(&ExplorerWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

				m_pViewedFolder.OnChanged() += std::bind(&ExplorerWindow::OnViewedFolderChanged, this, std::placeholders::_1, std::placeholders::_2);

				return BaseWindow::Initialize();
			}

			bool ExplorerWindow::Destroy()
			{
				core::EDITOR_TOOL->GetEditor().GetAssetDatabase().GetOnScanCompleted() -= std::bind(&ExplorerWindow::OnScanCompleted, this);

				core::EDITOR_TOOL->GetEditor().GetSelectable().OnChanged() -= std::bind(&ExplorerWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

				m_pViewedFolder.OnChanged() -= std::bind(&ExplorerWindow::OnViewedFolderChanged, this, std::placeholders::_1, std::placeholders::_2);

				return BaseWindow::Destroy();
			}

			void ExplorerWindow::RenderFolder(ExplorerFileUIView& a_Resource, int a_Indent, const ImVec2& a_InitialPos)
			{
				bool
					clicked = false,
					right_clicked = false;

				a_Resource.RenderTree(clicked, right_clicked, m_pViewedFolder.get() == &a_Resource, a_Indent, a_InitialPos, false /*highlighted if selected*/);

				if (clicked)
				{
					m_pViewedFolder = &a_Resource;
					m_bNeedsRefresh = true;

					SetSelectable(&a_Resource);
				}

				if (a_Resource.IsFoldedOut())
				{
					for (ExplorerFileUIView& child : a_Resource.GetChildren())
					{
						if (child.GetFileResource().GetAssetType() == gallus::editor::AssetType::Folder)
						{
							RenderFolder(child, a_Indent + 1, ImGui::GetCursorPos());
						}
					}
				}
			}

			void ExplorerWindow::Render()
			{
				if (!core::EDITOR_TOOL)
				{
					return;
				}

				if (m_bNeedsRescan)
				{
					m_pViewedFolder = nullptr;

					m_aFilteredExplorerItems.clear();
					m_aExplorerItems.clear();

					m_aExplorerItems.reserve(gallus::core::EDITOR_TOOL->GetEditor().GetAssetDatabase().GetRoot().GetChildren().size());
					for (gallus::editor::FileResource& fileResource : gallus::core::EDITOR_TOOL->GetEditor().GetAssetDatabase().GetRoot().GetChildren())
					{
						m_aExplorerItems.emplace_back(m_Window, fileResource);
					}
					m_bNeedsRescan = false;

					if (!m_PreviousViewedFolderPath.empty())
					{
						ExplorerFileUIView* viewedFolder = nullptr;
						for (ExplorerFileUIView& view : m_aExplorerItems)
						{
							view.SearchForPath(m_PreviousViewedFolderPath, viewedFolder);
						}
						m_pViewedFolder = viewedFolder;
					}
				}

				// This needs to be done at the start of the frame to avoid errors.
				// We refresh the assets that show up based on the search bar and the root directory.
				if (m_bNeedsRefresh && m_pViewedFolder.get())
				{
					m_aFilteredExplorerItems.clear();

					for (ExplorerFileUIView& view : m_pViewedFolder.get()->GetChildren())
					{
						if (m_SearchBar.GetString().empty() || string_extensions::StringToLower(view.GetFileResource().GetPath().filename().generic_string()).find(m_SearchBar.GetString()) != std::string::npos)
						{
							m_aFilteredExplorerItems.push_back(&view);
						}
					}

					if (!m_PreviousSelectablePath.empty())
					{
						ExplorerFileUIView* viewedFolder = nullptr;
						for (ExplorerFileUIView& view : m_aExplorerItems)
						{
							view.SearchForPath(m_PreviousSelectablePath, viewedFolder);
						}
						SetSelectable(viewedFolder);
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

				bool doRescan = false;
				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REFRESH), BUTTON_ID, "RESCAN_EXPLORER").c_str(), m_Window.GetHeaderSize(), m_Window.GetIconFont()))
				{
					doRescan = true;
				}

				ImGui::SameLine();

				bool list = m_ExplorerViewMode == ExplorerViewMode::ExplorerViewMode_List;
				bool grid = m_ExplorerViewMode == ExplorerViewMode::ExplorerViewMode_Grid;
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_LIST), BUTTON_ID, "LIST_EXPLORER").c_str(), &list, m_Window.GetHeaderSize(), m_Window.GetIconFont(), list ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
				{
					if (list)
					{
						m_ExplorerViewMode = ExplorerViewMode::ExplorerViewMode_List;
					}
				}

				ImGui::SameLine();

				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GRID), BUTTON_ID, "GRID_EXPLORER").c_str(), &grid, m_Window.GetHeaderSize(), m_Window.GetIconFont(), grid ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
				{
					if (grid)
					{
						m_ExplorerViewMode = ExplorerViewMode::ExplorerViewMode_Grid;
					}
				}

				ImVec2 endPos = ImGui::GetCursorPos();

				float searchbarWidth = 300;
				float inputPadding = m_Window.GetFramePadding().x / 2;
				ImVec2 searchBarPos = ImVec2(
					ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (searchbarWidth + m_Window.GetWindowPadding().x),
					(topPosY + (toolbarSize.y / 2)) - (((inputPadding * 2) + m_Window.GetFontSize()) / 2)
				);
				ImGui::SetCursorPos(searchBarPos);
				if (m_SearchBar.Render(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SEARCHBAR_EXPLORER").c_str(), ImVec2(searchbarWidth, toolbarSize.y), inputPadding))
				{
					m_bNeedsRefresh = true;
				}

				ImGui::SetCursorPos(endPos);

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::EndToolbar(ImVec2(m_Window.GetWindowPadding().x, 0));

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
				ImGui::SetCursorPosY(ImGui::GetCursorPos().y + m_Window.GetFramePadding().y);
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "DIRECTORIES_EXPLORER").c_str(),
					ImVec2(
					0,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX
					))
				{
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					for (ExplorerFileUIView& view : m_aExplorerItems)
					{
						RenderFolder(view, 0, ImGui::GetCursorPos());
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();

				std::string popUpID = ImGui::IMGUI_FORMAT_ID("", POPUP_WINDOW_ID, "RESOURCE_OPTIONS_EXPLORER");

				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_ExplorerViewMode == ExplorerViewMode::ExplorerViewMode_List ? ImVec2() : m_Window.GetWindowPadding());
				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_EXPLORER").c_str(),
					ImVec2(
					ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
					ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					ImGuiChildFlags_Borders
					))
				{
					if (m_pViewedFolder.get())
					{
						if (ImGui::BeginChild(
							ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_INNER_EXPLORER").c_str(),
							ImVec2(
							0,
							0
							)
							))
						{
							int count = m_pViewedFolder.get()->GetParent() ? 1 : 0;
							for (ExplorerFileUIView* view : m_aFilteredExplorerItems)
							{
								if (!view)
								{
									continue;
								}

								bool
									clicked = false,
									right_clicked = false,
									double_clicked = false;

								if (m_ExplorerViewMode == ExplorerViewMode::ExplorerViewMode_List)
								{
									view->RenderList(
										clicked,
										right_clicked,
										double_clicked,
										core::EDITOR_TOOL->GetEditor().GetSelectable().get() == view,
										false
									);
								}
								else
								{
									ImVec2 viewSize = m_Window.GetHeaderSize() * 2.0f;

									// Calculate the maximum number of icons that can fit horizontally as squares
									int iconsPerRow = static_cast<int>(ImGui::GetContentRegionAvail().x / viewSize.x);
									iconsPerRow = std::max(1, iconsPerRow); // Ensure at least 1 icon fits

									// Get the available space in the window
									view->RenderGrid(
										viewSize,
										clicked,
										right_clicked,
										double_clicked,
										core::EDITOR_TOOL->GetEditor().GetSelectable().get() == view,
										false
									);

									count++;
									if (count % iconsPerRow != 0)
									{
										ImGui::SameLine();
									}
								}

								if (double_clicked && view->GetFileResource().GetAssetType() == gallus::editor::AssetType::Folder)
								{
									m_pViewedFolder = view;
									m_bNeedsRefresh = true;
								}

								if (clicked)
								{
									core::EDITOR_TOOL->GetEditor().SetSelectable(view, new ExplorerFileInspectorView(m_Window, *view));
								}
							}
						}
						ImGui::EndChild();
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				if (doRescan)
				{
					const ExplorerFileUIView* derivedPtr = dynamic_cast<const ExplorerFileUIView*>(core::EDITOR_TOOL->GetEditor().GetSelectable().get());
					if (derivedPtr)
					{
						core::EDITOR_TOOL->GetEditor().SetSelectable(nullptr, nullptr);
					}

					core::EDITOR_TOOL->GetEditor().GetAssetDatabase().Rescan();
				}
			}

			void ExplorerWindow::SetSelectable(ExplorerFileUIView* a_pView)
			{
				core::EDITOR_TOOL->GetEditor().SetSelectable(a_pView, a_pView ? new ExplorerFileInspectorView(m_Window, *a_pView) : nullptr);
			}

			void ExplorerWindow::OnScanCompleted()
			{
				m_bNeedsRescan = true;
				m_bNeedsRefresh = true;
			}

			void ExplorerWindow::OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal)
			{
				if (!newVal)
				{
					return;
				}

				const ExplorerFileUIView* derivedPtr = dynamic_cast<const ExplorerFileUIView*>(newVal);
				if (!derivedPtr) // New selectable is NOT an explorer file, so we must reset the previous folder path.
				{
					m_PreviousSelectablePath = fs::path();
				}
				else // New selectable is an explorer file, set folder path.
				{
					m_PreviousSelectablePath = derivedPtr->GetFileResource().GetPath();
				}
			}

			void ExplorerWindow::OnViewedFolderChanged(const ExplorerFileUIView* oldVal, const ExplorerFileUIView* newVal)
			{
				if (!newVal)
				{
					return;
				}

				m_PreviousViewedFolderPath = newVal->GetFileResource().GetPath();
			}
		}
	}
}

#endif // IMGUI_DISABLE
#endif // _EDITOR