#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

#include <vector>
#include <string>
#include <memory>

// core includes
#include "core/Observable.h"

// utils includes
#include "utils/file_abstractions.h"

// graphics includes
#include "graphics/imgui/views/DataTypes/StringTextInput.h"

// graphics includes
#include "editor/graphics/imgui/selectables/FileEditorSelectable.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			enum class ExplorerViewMode
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
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ExplorerWindow(ImGuiWindow& a_Window);

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
				/// <summary>
				/// Renders a folder in the explorer.
				/// </summary>
				/// <param name="a_Resource">Pointer to the file resource.</param>
				/// <param name="a_Indent">Amount of indents.</param>
				/// <param name="a_InitialPos">The starting pos for indent 0.</param>
				void RenderFolder(FileEditorSelectable& a_Resource, int a_Indent, const ImVec2& a_InitialPos);

				void SetSelectable(FileEditorSelectable* a_pView);

				/// <summary>
				/// Callback function for when the scanning of the explorer has been completed.
				/// </summary>
				void OnScanCompleted();

				void OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal);

				void OnViewedFolderChanged(const FileEditorSelectable* oldVal, const FileEditorSelectable* newVal);

				ExplorerViewMode m_ExplorerViewMode = ExplorerViewMode::ExplorerViewMode_List; // How are explorer resources shown?

				bool m_bNeedsRescan = true; /// Whether the explorer needs to refresh the results shown in the explorer window.
				bool m_bNeedsRefresh = true; /// Whether the explorer needs to refresh the results shown in the explorer window.

				std::vector<FileEditorSelectable> m_aExplorerItems;
				std::vector<FileEditorSelectable*> m_aFilteredExplorerItems;

				core::Observable<FileEditorSelectable*> m_pViewedFolder; /// Selected resource used for context menu.

				SearchBarInput m_SearchBar; /// Search bar to filter specific explorer items in the explorer window.

				fs::path m_PreviousSelectablePath;
				fs::path m_PreviousViewedFolderPath;
			};
		}
	}
}

#endif // IMGUI_DISABLE
#endif // _EDITOR