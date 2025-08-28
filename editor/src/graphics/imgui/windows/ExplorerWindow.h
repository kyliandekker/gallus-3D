#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "graphics/imgui/windows/BaseWindow.h"
#include "graphics/imgui/views/DataTypes/StringTextInput.h"
#include "graphics/imgui/views/ExplorerFileUIView.h"
#include "utils/file_abstractions.h"
#include "core/Observable.h"

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
				void RenderFolder(ExplorerFileUIView& a_Resource, int a_Indent, const ImVec2& a_InitialPos);

				void SetSelectable(ExplorerFileUIView* a_pView);

				/// <summary>
				/// Callback function for when the scanning of the explorer has been completed.
				/// </summary>
				void OnScanCompleted();

				void OnSelectableChanged(const EditorSelectable* oldVal, const EditorSelectable* newVal);

				void OnViewedFolderChanged(const ExplorerFileUIView* oldVal, const ExplorerFileUIView* newVal);

				ExplorerViewMode m_ExplorerViewMode = ExplorerViewMode::ExplorerViewMode_List; // How are explorer resources shown?

				bool m_bNeedsRescan = true; /// Whether the explorer needs to refresh the results shown in the explorer window.
				bool m_bNeedsRefresh = true; /// Whether the explorer needs to refresh the results shown in the explorer window.

				std::vector<ExplorerFileUIView> m_aExplorerItems;
				std::vector<ExplorerFileUIView*> m_aFilteredExplorerItems;

				core::Observable<ExplorerFileUIView*> m_pViewedFolder; /// Selected resource used for context menu.

				SearchBarInput m_SearchBar; /// Search bar to filter specific explorer items in the explorer window.

				fs::path m_PreviousSelectablePath;
				fs::path m_PreviousViewedFolderPath;
			};
		}
	}
}

#endif // IMGUI_DISABLE
#endif // _EDITOR