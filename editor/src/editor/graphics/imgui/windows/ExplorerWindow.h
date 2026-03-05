#pragma once

#include "imgui_system/windows/BaseWindow.h"

// external
#include <vector>
#include <string>
#include <memory>

// core
#include "core/Observable.h"

// editor
#include "editor/graphics/imgui/views/selectables/FileEditorSelectable.h"
#include "editor/graphics/imgui/views/selectables/FolderEditorSelectable.h"
#include "editor/graphics/imgui/views/Toolbar.h"

namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	enum ExplorerViewMode
	{
		ExplorerViewMode_List,
		ExplorerViewMode_Grid
	};

	//---------------------------------------------------------------------
	// ExplorerWindow
	//---------------------------------------------------------------------
	/// <summary>
	/// A window that displays and manages the asset database of the project.
	/// Allows navigation, inspection, and organization of assets in a folder-like structure.
	/// </summary>
	class ExplorerWindow : public BaseWindow
	{
	public:
		/// <summary>
		/// Constructs an explorer window.
		/// </summary>
		/// <param name="a_System">The ImGui System for rendering the view.</param>
		ExplorerWindow(ImGuiSystem& a_System);

		/// <summary>
		/// Initializes all values and behaviours associated with the explorer window.
		/// </summary>
		/// <returns>True if initialization is successful, otherwise false.</returns>
		bool Initialize() override;

		/// <summary>
		/// Destroys and disables the explorer window.
		/// </summary>
		/// <returns>True if destruction is successful, otherwise false.</returns>
		bool Destroy() override;

		/// <summary>
		/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
		/// </summary>
		void Update() override;

		/// <summary>
		/// Renders the explorer window.
		/// </summary>
		void Render() override;
	private:
		// Toolbar.
		void PopulateToolbar();
		void DrawToolbar();

		// Folder drawing.
		void DrawFolders();
		void DrawFiles();

		/// <summary>
		/// Renders a folder in the explorer.
		/// </summary>
		/// <param name="a_Resource">Pointer to the file resource.</param>
		/// <param name="a_Indent">Amount of indents.</param>
		/// <param name="a_InitialPos">The starting pos for indent 0.</param>
		void RenderFolder(FolderEditorSelectable& a_Resource, int a_Indent, const ImVec2& a_InitialPos);

		/// <summary>
		/// Callback function for when the scanning of the explorer has been completed.
		/// </summary>
		void OnScanCompleted();

		/// <summary>
		/// Adds folders to a file resource's vector.
		/// </summary>
		/// <param name="a_aVector">The vector of child resources.</param>
		/// <param name="a_Folder">The folder that gets scanned.</param>
		void AddFoldersToVector(std::vector<FolderEditorSelectable>& a_aVector, file::FileResource& a_Folder);

		void OnSelectableChanged(std::weak_ptr<EditorSelectable> a_pOldValue, std::weak_ptr<EditorSelectable> a_pNewValue);

		// Folders.
		std::vector<FolderEditorSelectable> m_aFolders;

		// List items.
		std::vector<std::shared_ptr<FileEditorSelectable>> m_aListItems;
		std::vector<std::weak_ptr<FileEditorSelectable>> m_aFilteredListItems;
		
		std::string m_sSelectedFolder = "";
		std::string m_sPreviousSelectedFolder = "";

		std::string m_sPreviousSelectableName = "";

		std::string m_sSearchBarText;
		bool m_bNeedsRescan = true; /// Whether the explorer needs to rescan the entire assets folder.
		bool m_bNeedsRootRefresh = true; /// Whether the explorer needs to filter specific assets out of the asset folder.
		bool m_bNeedsRefresh = true; /// Whether the explorer needs to filter specific assets out of the asset folder.

		Toolbar m_Toolbar;
	};
}