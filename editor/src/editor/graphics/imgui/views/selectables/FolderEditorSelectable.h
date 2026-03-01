#pragma once

// external
#include <imgui/imgui.h>
#include <string>
#include <vector>

// utils
#include "file/file_abstractions.h"

// editor
#include "editor/graphics/imgui/views/selectables/BaseFileEditorSelectable.h"

namespace gallus::file
{
	class FileResource;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	class FolderEditorSelectable : public BaseFileEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file selectable.
		/// </summary>
		FolderEditorSelectable(ImGuiSystem& a_System, file::FileResource& a_FileResource);

		std::vector<FolderEditorSelectable>& GetChildren()
		{
			return m_aChildren;
		}

		/// <summary>
		/// Checks if the resource has sub folders.
		/// Override to specify folder support for the resource.
		/// </summary>
		/// <returns>True if the resource has sub folders, otherwise false.</returns>
		bool HasFolders() const;

		/// <summary>
		/// Checks if the resource node is folded out in the UI.
		/// </summary>
		/// <returns>True if the node is folded out, otherwise false.</returns>
		bool IsFoldedOut() const;

		/// <summary>
		/// Sets the folded-out state of the resource node.
		/// </summary>
		/// <param name="a_FoldedOut">True to fold out, false to collapse.</param>
		void SetFoldedOut(bool a_bIsFoldedOut);

		/// <summary>
		/// Renders the file resource in the UI in the tree.
		/// </summary>
		/// <param name="a_bClicked">Indicates if the resource was clicked.</param>
		/// <param name="a_bRightClicked">Indicates if the resource was right-clicked.</param>
		/// <param name="a_bSelected">Indicates if the resource is selected.</param>
		/// <param name="a_iIndent">The amount of indents.</param>
		/// <param name="a_vInitialPos">The starting position for 0 indent.</param>
		/// <param name="a_bInContextMenu">Indicates if the resource is currently open in the context menu.</param>
		void RenderTree(bool& a_bClicked, bool& a_bRightClicked, bool a_bSelected, int a_iIndent, const ImVec2& a_vInitialPos, bool a_bInContextMenu);
	private:
		std::vector<FolderEditorSelectable> m_aChildren;

		bool m_bIsFoldedOut = false; /// Whether the node is folded out.
	};
}