#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// external
#include <imgui/imgui.h>
#include <string>
#include <vector>

// utils
#include "utils/file_abstractions.h"

// editor
#include "editor/graphics/imgui/EditorSelectable.h"

namespace gallus
{
	namespace resources
	{
		class FileResource;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;
			class FileEditorSelectables;

			class FileEditorSelectable : public EditorSelectable
			{
			public:
				FileEditorSelectable(ImGuiWindow& a_Window, gallus::resources::FileResource& a_FileResource, FileEditorSelectable* a_pParent = nullptr, bool a_bGetChildren = true);

				void Select() override;

				void Deselect() override;

				/// <summary>
				/// Retrieves the icon string for the resource.
				/// </summary>
				/// <returns>A reference to the icon string.</returns>
				std::string GetIcon() const override;

				void SetIcon();

				/// <summary>
				/// Sets the icon string for the resource.
				/// </summary>
				void SetIcon(const std::string& a_sIcon);

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

				/// <summary>
				/// Renders the file resource in the UI.
				/// </summary>
				/// <param name="a_bClicked">Indicates if the resource was clicked.</param>
				/// <param name="a_bRightClicked">Indicates if the resource was right-clicked.</param>
				/// <param name="a_bDoubleClicked">Indicates if the resource was double-clicked.</param>
				/// <param name="a_bSelected">Indicates if the resource is selected.</param>
				/// <param name="a_bInContextMenu">Indicates if the resource is currently open in the context menu.</param>
				void RenderList(bool& a_bClicked, bool& a_bRightClicked, bool& a_bDoubleClicked, bool a_bSelected, bool a_bInContextMenu);

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
				/// Checks if the resource has sub folders.
				/// Override to specify folder support for the resource.
				/// </summary>
				/// <returns>True if the resource has sub folders, otherwise false.</returns>
				bool HasFolders() const;

				void Render() override
				{}

				std::vector<FileEditorSelectable>& GetChildren()
				{
					return m_aChildren;
				}

				gallus::resources::FileResource& GetFileResource()
				{
					return m_FileResource;
				}

				const gallus::resources::FileResource& GetFileResource() const
				{
					return m_FileResource;
				}

				FileEditorSelectable* GetParent()
				{
					return m_pParent;
				}

				void SetDisplayName(const std::string& a_sDisplayName)
				{
					m_sDisplayName = a_sDisplayName;
				}

				bool SearchForPath(const fs::path& a_Path, FileEditorSelectable*& a_pExplorerFile);

				void OnRename(const std::string& a_sName) override;

				void OnDelete() override;

				void OnShowInExplorer() override;

				std::string GetName() const override;

				void OnDoubleClicked() override;

				void RenderEditorFields() override;
			private:
				gallus::resources::FileResource& m_FileResource;
				std::string m_sDisplayName; /// The name that will be displayed in imgui.
				std::string m_sIcon; /// The icon of the resource.
				bool m_bIsFoldedOut = false; /// Whether the node is folded out.

				FileEditorSelectables* m_pFileEditorSelectable = nullptr;

				FileEditorSelectable* m_pParent = nullptr;
				std::vector<FileEditorSelectable> m_aChildren;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE