#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerWindow.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

// utils
#include "utils/string_extensions.h"
#include "utils/file_abstractions.h"

// resources
#include "resources/FileResource.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/selectables/FileEditorSelectable.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// ExplorerWindow
			//---------------------------------------------------------------------
			ExplorerWindow::ExplorerWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_FOLDER) + " Explorer", "Explorer")
			{}

			//---------------------------------------------------------------------
			bool ExplorerWindow::Initialize()
			{
				core::EDITOR_ENGINE->GetEditor().GetAssetDatabase().GetOnScanCompleted() += std::bind(&ExplorerWindow::OnScanCompleted, this);

				core::EDITOR_ENGINE->GetEditor().GetSelectable().OnChanged() += std::bind(&ExplorerWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

				m_pViewedFolder.OnChanged() += std::bind(&ExplorerWindow::OnViewedFolderChanged, this, std::placeholders::_1, std::placeholders::_2);
				
				PopulateToolbar();

				return BaseWindow::Initialize();
			}

			//---------------------------------------------------------------------
			bool ExplorerWindow::Destroy()
			{
				core::EDITOR_ENGINE->GetEditor().GetAssetDatabase().GetOnScanCompleted() -= std::bind(&ExplorerWindow::OnScanCompleted, this);

				core::EDITOR_ENGINE->GetEditor().GetSelectable().OnChanged() -= std::bind(&ExplorerWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

				m_pViewedFolder.OnChanged() -= std::bind(&ExplorerWindow::OnViewedFolderChanged, this, std::placeholders::_1, std::placeholders::_2);

				return BaseWindow::Destroy();
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::Update()
			{
				if (core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFullScreenPlayMode())
				{
					return;
				}
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::PopulateToolbar()
			{
				ImVec2 toolbarSize = ImVec2(0, m_Window.GetHeaderSize().y);
				m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_EXPLORER"), toolbarSize);

				// Rescan button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarSearchbar(m_Window,
					ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SEARCHBAR_EXPLORER"),
					200,
					[this](const std::string& a_sResult)
					{
						m_sSearchBarText = a_sResult;
						m_bNeedsRefresh = true;
					}
				));

				// Rescan button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						if (ImGui::IconButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REFRESH), BUTTON_ID, "RESCAN_EXPLORER").c_str(), "Triggers a full rescan of the asset database, updating all folders and files in the explorer.", m_Window.GetHeaderSize()))
						{
							m_bNeedsRescan = true;
						}
					}
				));

				// List button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();
						bool list = editorSettings.GetExplorerViewMode() == ExplorerViewMode::ExplorerViewMode_List;
						if (ImGui::IconCheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_LIST), BUTTON_ID, "LIST_EXPLORER").c_str(), &list, "Switches the explorer display to a vertical list of items, showing file names and metadata in a single column.", m_Window.GetHeaderSize(), list ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
						{
							if (list)
							{
								editorSettings.SetExplorerViewMode(ExplorerViewMode::ExplorerViewMode_List);
								editorSettings.Save();
							}
						}
					}
				));

				// Grid button.
				m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_Window,
					[this]()
					{
						editor::EditorSettings& editorSettings = core::EDITOR_ENGINE->GetEditor().GetEditorSettings();
						bool grid = editorSettings.GetExplorerViewMode() == ExplorerViewMode::ExplorerViewMode_Grid;
						if (ImGui::IconCheckboxButton(
							ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GRID), BUTTON_ID, "GRID_EXPLORER").c_str(), &grid, "Switches the explorer display to a grid of icons, useful for browsing visually.", m_Window.GetHeaderSize(), grid ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
						{
							if (grid)
							{
								editorSettings.SetExplorerViewMode(ExplorerViewMode::ExplorerViewMode_Grid);
								editorSettings.Save();
							}
						}
					}
				));
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::DrawToolbar()
			{
				// Start toolbar.
				m_Toolbar.StartToolbar();

				// Render toolbar.
				m_Toolbar.Render();

				// End toolbar.
				m_Toolbar.EndToolbar();
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::DrawFolders()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
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
					for (FileEditorSelectable& view : m_aExplorerItems)
					{
						RenderFolder(view, 0, ImGui::GetCursorPos());
					}
					ImGui::PopStyleVar();
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::DrawFiles()
			{
				int viewMode = core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetExplorerViewMode();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, viewMode == ExplorerViewMode::ExplorerViewMode_List ? ImVec2() : m_Window.GetWindowPadding());
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
							for (FileEditorSelectable* view : m_aFilteredExplorerItems)
							{
								if (!view)
								{
									continue;
								}

								// store these outside of view mode because both use them.
								bool
									clicked = false,
									right_clicked = false,
									double_clicked = false;

								if (viewMode == ExplorerViewMode::ExplorerViewMode_List)
								{
									view->RenderList(
										clicked,
										right_clicked,
										double_clicked,
										core::EDITOR_ENGINE->GetEditor().GetSelectable().get() == view,
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
										core::EDITOR_ENGINE->GetEditor().GetSelectable().get() == view,
										false
									);

									count++;
									if (count % iconsPerRow != 0)
									{
										ImGui::SameLine();
									}
								}

								if (double_clicked)
								{
									if (view->GetFileResource().GetAssetType() == gallus::resources::AssetType::Folder)
									{
										m_pViewedFolder = view;
										m_bNeedsRefresh = true;
									}
									else
									{
										view->OnDoubleClicked();
									}
								}

								if (clicked)
								{
									core::EDITOR_ENGINE->GetEditor().SetSelectable(view);
								}
							}
						}
						ImGui::EndChild();
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}
			
			//---------------------------------------------------------------------
			void ExplorerWindow::RenderFolder(FileEditorSelectable& a_Resource, int a_Indent, const ImVec2& a_InitialPos)
			{
				if (a_Resource.GetFileResource().GetAssetType() != gallus::resources::AssetType::Folder)
				{
					return;
				}

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
					for (FileEditorSelectable& child : a_Resource.GetChildren())
					{
						RenderFolder(child, a_Indent + 1, ImGui::GetCursorPos());
					}
				}
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				if (m_bNeedsRescan)
				{
					m_pViewedFolder = nullptr;

					m_aFilteredExplorerItems.clear();
					m_aExplorerItems.clear();

					m_aExplorerItems.reserve(gallus::core::EDITOR_ENGINE->GetResourceAtlas().GetResourceFolder().GetChildren().size());
					for (gallus::resources::FileResource& fileResource : gallus::core::EDITOR_ENGINE->GetResourceAtlas().GetResourceFolder().GetChildren())
					{
						m_aExplorerItems.emplace_back(m_Window, fileResource);
					}
					m_bNeedsRescan = false;

					if (!m_PreviousViewedFolderPath.empty())
					{
						FileEditorSelectable* viewedFolder = nullptr;
						for (FileEditorSelectable& view : m_aExplorerItems)
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

					std::string searchString = string_extensions::StringToLower(m_sSearchBarText);
					for (FileEditorSelectable& view : m_pViewedFolder.get()->GetChildren())
					{
						if (searchString.empty() || string_extensions::StringToLower(view.GetFileResource().GetPath().filename().generic_string()).find(searchString) != std::string::npos)
						{
							m_aFilteredExplorerItems.push_back(&view);
						}
					}

					if (!m_PreviousSelectablePath.empty())
					{
						FileEditorSelectable* viewedFolder = nullptr;
						for (FileEditorSelectable& view : m_aExplorerItems)
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

				bool isDelete = ImGui::IsKeyPressed(ImGuiKey_Delete);
				if (isDelete)
				{
					if (graphics::imgui::FileEditorSelectable* file = dynamic_cast<graphics::imgui::FileEditorSelectable*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get()))
					{
						if (file)
						{
							file->OnDelete();
						}
					}
				}

				DrawToolbar();

				ImGui::SetCursorPos({
					ImGui::GetCursorPos().x + m_Window.GetFramePadding().x,
					ImGui::GetCursorPos().y + m_Window.GetFramePadding().y,
				});

				DrawFolders();
				ImGui::SameLine();
				DrawFiles();

				if (m_bNeedsRescan)
				{
					const FileEditorSelectable* derivedPtr = dynamic_cast<const FileEditorSelectable*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get());
					if (derivedPtr)
					{
						core::EDITOR_ENGINE->GetEditor().SetSelectable(nullptr);
					}

					core::EDITOR_ENGINE->GetEditor().GetAssetDatabase().Rescan();
				}
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::SetSelectable(FileEditorSelectable* a_pView)
			{
				core::EDITOR_ENGINE->GetEditor().SetSelectable(a_pView);
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::OnScanCompleted()
			{
				m_bNeedsRescan = true;
				m_bNeedsRefresh = true;
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal)
			{
				if (!newVal)
				{
					return;
				}

				const FileEditorSelectable* derivedPtr = dynamic_cast<const FileEditorSelectable*>(newVal);
				if (!derivedPtr) // New selectable is NOT an explorer file, so we must reset the previous folder path.
				{
					m_PreviousSelectablePath = fs::path();
				}
				else // New selectable is an explorer file, set folder path.
				{
					m_PreviousSelectablePath = derivedPtr->GetFileResource().GetPath();
				}
			}

			//---------------------------------------------------------------------
			void ExplorerWindow::OnViewedFolderChanged(const FileEditorSelectable* oldVal, const FileEditorSelectable* newVal)
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