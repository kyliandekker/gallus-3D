#include "./ExplorerWindow.h"

#undef max

// external
#include <imgui/imgui_helpers.h>

#include "utils/string_extensions.h"

// imgui
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/AssetDatabase.h"
#include "editor/Editor.h"

// editor/file
#include "editor/file/FileResource.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// ExplorerWindow
	//---------------------------------------------------------------------
	ExplorerWindow::ExplorerWindow(ImGuiSystem& a_System) : BaseWindow(a_System, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_FOLDER) + " Explorer", "Explorer")
	{}

	//---------------------------------------------------------------------
	bool ExplorerWindow::Initialize()
	{
		GetEditorEngine().GetEditor()->OnSelectableChanged() += std::bind(&ExplorerWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

		GetEditorEngine().GetAssetDatabase()->GetOnScanCompleted() += std::bind(&ExplorerWindow::OnScanCompleted, this);

		PopulateToolbar();

		return BaseWindow::Initialize();
	}

	//---------------------------------------------------------------------
	bool ExplorerWindow::Destroy()
	{
		GetEditorEngine().GetEditor()->OnSelectableChanged() -= std::bind(&ExplorerWindow::OnSelectableChanged, this, std::placeholders::_1, std::placeholders::_2);

		GetEditorEngine().GetAssetDatabase()->GetOnScanCompleted() -= std::bind(&ExplorerWindow::OnScanCompleted, this);

		return BaseWindow::Destroy();
	}

	//---------------------------------------------------------------------
	void ExplorerWindow::Update()
	{
		if (GetEditorEngine().GetEditor()->GetEditorSettings().GetEditorState() != editor::EditorState::EditorState_Workspace)
		{
			return;
		}
		BaseWindow::Update();
	}

	//---------------------------------------------------------------------
	void ExplorerWindow::PopulateToolbar()
	{
		ImVec2 toolbarSize = ImVec2(0, m_System.GetHeaderSize().y);
		m_Toolbar = Toolbar(ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "TOOLBAR_EXPLORER"), toolbarSize);

		// Rescan button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarSearchbar(m_System,
			ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "SEARCHBAR_EXPLORER"),
			200,
			[this](const std::string& a_sResult)
			{
				m_sSearchBarText = a_sResult;
				m_bNeedsRefresh = true;
			}
		));

		// Rescan button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
			[this]()
			{
				if (ImGui::IconButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_REFRESH), BUTTON_ID, "RESCAN_EXPLORER").c_str(), "Triggers a full rescan of the asset database, updating all folders and files in the explorer.", m_System.GetHeaderSize()))
				{
					m_bNeedsRescan = true;
				}
			}
		));

		// List button.
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();
				bool list = editorSettings.GetExplorerViewMode() == ExplorerViewMode::ExplorerViewMode_List;
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_LIST), BUTTON_ID, "LIST_EXPLORER").c_str(), &list, "Switches the explorer display to a vertical list of items, showing file names and metadata in a single column.", m_System.GetHeaderSize(), list ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
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
		m_Toolbar.m_aToolbarItems.emplace_back(new ToolbarButton(m_System,
			[this]()
			{
				editor::EditorSettings& editorSettings = GetEditorEngine().GetEditor()->GetEditorSettings();
				bool grid = editorSettings.GetExplorerViewMode() == ExplorerViewMode::ExplorerViewMode_Grid;
				if (ImGui::IconCheckboxButton(
					ImGui::IMGUI_FORMAT_ID(std::string(font::ICON_GRID), BUTTON_ID, "GRID_EXPLORER").c_str(), &grid, "Switches the explorer display to a grid of icons, useful for browsing visually.", m_System.GetHeaderSize(), grid ? ImGui::GetStyleColorVec4(ImGuiCol_TextColorAccent) : ImGui::GetStyleColorVec4(ImGuiCol_Text)))
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
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_System.GetFramePadding());
		if (ImGui::BeginChild(
			ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "DIRECTORIES_EXPLORER").c_str(),
			ImVec2(
			0,
			ImGui::GetContentRegionAvail().y - m_System.GetFramePadding().y
			),
			ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX
		))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			for (FolderEditorSelectable& view : m_aFolders)
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
		int viewMode = GetEditorEngine().GetEditor()->GetEditorSettings().GetExplorerViewMode();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, viewMode == ExplorerViewMode::ExplorerViewMode_List ? ImVec2() : m_System.GetFramePadding());
		if (ImGui::BeginChild(
			ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_EXPLORER").c_str(),
			ImVec2(
			ImGui::GetContentRegionAvail().x - m_System.GetFramePadding().x,
			ImGui::GetContentRegionAvail().y - m_System.GetFramePadding().y
			),
			ImGuiChildFlags_Borders
		))
		{
			if (ImGui::BeginChild(
				ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "FILES_INNER_EXPLORER").c_str(),
				ImVec2(
				0,
				0
				)
				))
			{
				bool showTable = false;
				if (viewMode == ExplorerViewMode::ExplorerViewMode_List)
				{
					ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, m_System.GetFramePadding());
					showTable = ImGui::BeginTable(
						ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "EXPLORER_LIST_ITEMS").c_str(),
						4,
						ImGuiTableFlags_RowBg |
						ImGuiTableFlags_SizingFixedFit |
						ImGuiTableFlags_BordersInnerH |
						ImGuiTableFlags_BordersInnerV
					);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 0);
					ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
					ImGui::TableSetupColumn("AssetType", ImGuiTableColumnFlags_WidthStretch);

					ImGui::TableHeadersRow();
				}

				int count = 0;
				for (std::weak_ptr<FileEditorSelectable>& view : m_aFilteredListItems)
				{
					bool
						clicked = false,
						rightClicked = false,
						doubleClicked = false;

					std::shared_ptr<FileEditorSelectable> explorerItem = view.lock();
					if (!explorerItem)
					{
						continue;
					}

					if (viewMode == ExplorerViewMode::ExplorerViewMode_List && showTable)
					{
						ImGui::TableNextRow();

						bool selected = GetEditorEngine().GetEditor()->IsSelectable(explorerItem);
						if (selected)
						{
							ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
								ImGui::GetColorU32(ImGuiCol_HeaderActive));
						}

						ImGui::TableNextColumn();

						ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));          // disables background
						ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));   // disables hover highlight
						ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));    // disables pressed highlight

						// This spans the entire row internally.
						if (ImGui::Selectable(
							ImGui::IMGUI_FORMAT_ID("", SELECTABLE_ID, explorerItem->GetFileResource().GetPath().generic_string()).c_str(),
							selected,
							ImGuiSelectableFlags_SpanAllColumns |
							ImGuiSelectableFlags_AllowItemOverlap,
							ImVec2(0.0f, 0.0f)
						))
						{
							clicked = true;
						}

						ImGui::PopStyleColor();
						ImGui::PopStyleColor();
						ImGui::PopStyleColor();

						explorerItem->EnableDragFunctionality();

						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
						{
							doubleClicked = true;
						}

						ImGui::TableNextColumn();
						explorerItem->RenderList(selected);
					}
					else
					{
						ImVec2 viewSize = m_System.GetHeaderSize() * 2.0f;

						int iconsPerRow = static_cast<int>(ImGui::GetContentRegionAvail().x / viewSize.x);
						iconsPerRow = std::max(1, iconsPerRow);

						explorerItem->RenderGrid(
							viewSize,
							clicked,
							rightClicked,
							doubleClicked,
							GetEditorEngine().GetEditor()->IsSelectable(explorerItem),
							false
						);

						count++;
						if (count % iconsPerRow != 0)
						{
							ImGui::SameLine();
						}
					}

					if (clicked)
					{
						GetEditorEngine().GetEditor()->SetSelectable(view);
					}
					if (doubleClicked)
					{
						GetEditorEngine().GetEditor()->SetSelectable(view);

						if (explorerItem->GetFileResource().GetAssetType() == resources::AssetType::Folder)
						{
							m_sSelectedFolder = explorerItem->GetFileResource().GetName();
						}
						else
						{
							explorerItem->OnDoubleClicked();
						}
					}

				}
				if (viewMode == ExplorerViewMode::ExplorerViewMode_List)
				{
					ImGui::EndTable();
					ImGui::PopStyleVar();
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	
	//---------------------------------------------------------------------
	void ExplorerWindow::RenderFolder(FolderEditorSelectable& a_Resource, int a_Indent, const ImVec2& a_InitialPos)
	{
		if (a_Resource.GetFileResource().GetAssetType() != resources::AssetType::Folder)
		{
			return;
		}

		bool
			clicked = false,
			right_clicked = false;

		a_Resource.RenderTree(clicked, right_clicked, a_Resource.GetFileResource().GetName() == m_sSelectedFolder, a_Indent, a_InitialPos, false /*highlighted if selected*/);

		if (clicked)
		{
			m_sSelectedFolder = a_Resource.GetFileResource().GetName();
		}

		if (a_Resource.IsFoldedOut())
		{
			for (FolderEditorSelectable& child : a_Resource.GetChildren())
			{
				RenderFolder(child, a_Indent + 1, ImGui::GetCursorPos());
			}
		}
	}

	//---------------------------------------------------------------------
	void ExplorerWindow::AddFoldersToVector(std::vector<FolderEditorSelectable>& a_aVector, file::FileResource& a_Folder)
	{
		for (file::FileResource& fileResource : a_Folder.GetChildren())
		{
			if (fileResource.GetAssetType() == resources::AssetType::Folder)
			{
				a_aVector.emplace_back(m_System, fileResource);

				FolderEditorSelectable& fileResource = a_aVector[a_aVector.size() - 1];
				AddFoldersToVector(fileResource.GetChildren(), fileResource.GetFileResource());
			}
		}
	}

	//---------------------------------------------------------------------
	void ExplorerWindow::Render()
	{
		editor::AssetDatabase* assetDatabase = GetEditorEngine().GetAssetDatabase();
		if (!assetDatabase)
		{
			return;
		}

		// This gets called whenever the asset database rescans.
		if (m_bNeedsRescan)
		{
			m_aFilteredListItems.clear();
			m_aFolders.clear();
			m_aListItems.clear();

			// Adds all folders from the root folder.
			AddFoldersToVector(m_aFolders, *GetEditorEngine().GetAssetDatabase()->GetResourceFolder());

			m_bNeedsRescan = false;
			m_bNeedsRootRefresh = true;
		}

		if (m_bNeedsRootRefresh || m_sPreviousSelectedFolder != m_sSelectedFolder)
		{
			m_aFilteredListItems.clear();
			m_aListItems.clear();

			// Adds all the items in the current selected folder.
			file::FileResource* fileResource = nullptr;
			if (!m_sSelectedFolder.empty() && GetEditorEngine().GetAssetDatabase()->GetResourceFolder()->Find(fs::path(m_sSelectedFolder), fileResource))
			{
				if (fileResource)
				{
					size_t count = fileResource->GetChildren().size();
					if (fileResource->GetParent() != GetEditorEngine().GetAssetDatabase()->GetResourceFolder())
					{
						count++;
						m_aListItems.emplace_back(std::make_shared<FileEditorSelectable>(m_System, *fileResource->GetParent()));
						m_aListItems[m_aListItems.size() - 1]->SetName("../");
						m_aListItems[m_aListItems.size() - 1]->SetShowRename(false);
						m_aListItems[m_aListItems.size() - 1]->SetShowDelete(false);
						m_aListItems[m_aListItems.size() - 1]->SetShowShowInExplorer(false);
					}

					m_aListItems.reserve(count);
					for (file::FileResource& fileResource : fileResource->GetChildren())
					{
						m_aListItems.emplace_back(std::make_shared<FileEditorSelectable>(m_System, fileResource));
						if (fileResource.GetName() == m_sPreviousSelectableName)
						{
							GetEditorEngine().GetEditor()->SetSelectable(m_aListItems[m_aListItems.size() - 1]);
						}
					}
				}
			}

			m_bNeedsRootRefresh = false;
			m_bNeedsRefresh = true;
			m_sPreviousSelectedFolder = m_sSelectedFolder;
		}

		// We push assets to a different "filtered" array based on the search bar.
		if (m_bNeedsRefresh)
		{
			m_aFilteredListItems.clear();

			std::string searchString = string_extensions::StringToLower(m_sSearchBarText);

			for (std::shared_ptr<FileEditorSelectable> view : m_aListItems)
			{
				if (searchString.empty() || string_extensions::StringToLower(view->GetFileResource().GetPath().filename().generic_string()).find(searchString) != std::string::npos)
				{
					m_aFilteredListItems.push_back(view);
				}
			}

			m_bNeedsRefresh = false;
		}

		DrawToolbar();

		ImGui::SetCursorPos({
			ImGui::GetCursorPos().x + m_System.GetFramePadding().x,
			ImGui::GetCursorPos().y + m_System.GetFramePadding().y,
		});

		DrawFolders();
		ImGui::SameLine();
		DrawFiles();

		if (m_bNeedsRescan)
		{
			GetEditorEngine().GetAssetDatabase()->Rescan();
		}
	}

	//---------------------------------------------------------------------
	void ExplorerWindow::OnScanCompleted()
	{
		m_bNeedsRescan = true;
		m_bNeedsRefresh = true;
	}

	//---------------------------------------------------------------------
	void ExplorerWindow::OnSelectableChanged(std::weak_ptr<EditorSelectable> a_pOldValue, std::weak_ptr<EditorSelectable> a_pNewValue)
	{
		std::shared_ptr<EditorSelectable> selectable = a_pNewValue.lock();

		if (!selectable)
		{
			return;
		}

		const FileEditorSelectable* derivedPtr = dynamic_cast<const FileEditorSelectable*>(selectable.get());
		if (!derivedPtr) // New selectable is NOT an explorer file, so we must reset the previous folder path.
		{
			m_sPreviousSelectableName = "";
		}
		else // New selectable is an explorer file, set folder path.
		{
			m_sPreviousSelectableName = derivedPtr->GetFileResource().GetName();
		}
	}
}