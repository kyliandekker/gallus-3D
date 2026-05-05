#pragma once

// external
#include <imgui/imgui.h>
#include <string>
#include <vector>

// editor
#include "editor/graphics/imgui/views/selectables/BaseFileEditorSelectable.h"

namespace gallus::file
{
	class FileResource;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;
	class FileTypeEditorSelectable;

	class FileEditorSelectable : public BaseFileEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file selectable.
		/// </summary>
		FileEditorSelectable(ImGuiSystem& a_System, file::FileResource& a_FileResource);

		/// <summary>
		/// Selects the editor selectable.
		/// </summary>
		void OnSelect() override;

		/// <summary>
		/// Deselects the editor selectable.
		/// </summary>
		void OnDeselect() override;

		/// <summary>
		/// Renders the file resource in the UI in a grid way.
		/// </summary>
		/// <param name="m_vSize">The size of the grid item.</param>
		/// <param name="a_bClicked">Indicates if the resource was clicked.</param>
		/// <param name="a_bRightClicked">Indicates if the resource was right-clicked.</param>
		/// <param name="a_bDoubleClicked">Indicates if the resource was double-clicked.</param>
		/// <param name="a_bSelected">Indicates if the resource is selected.</param>
		/// <param name="a_bInContextMenu">Indicates if the resource is currently open in the context menu.</param>
		void RenderGrid(const ImVec2& m_vSize, bool& a_bClicked, bool& a_bRightClicked, bool& a_bDoubleClicked, bool a_bSelected, bool a_bInContextMenu);

		/// <summary>
		/// Called when the selectable gets double clicked.
		/// </summary>
		void OnDoubleClicked() override;

		/// <summary>
		/// Renders the specific file properties.
		/// </summary>
		/// <returns>True if a field gets changed, otherwise false.</returns>
		bool RenderEditorFields() override;

		void EnableDragFunctionality();
	protected:
		FileTypeEditorSelectable* m_pFileEditorSelectable = nullptr;
	};
}