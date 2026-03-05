#pragma once

#include "imgui_system/views/ImGuiUIView.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <string>

namespace gallus::graphics::imgui
{
	class EditorSelectable : public ImGuiUIView
	{
	public:
		EditorSelectable(ImGuiSystem& a_System) : ImGuiUIView(a_System)
		{}

		/// <summary>
		/// Renders the specific file properties.
		/// </summary>
		/// <returns>True if a field gets changed, otherwise false.</returns>
		virtual bool RenderEditorFields()
		{
			return false;
		}

		/// <summary>
		/// Renders the specific file gizmos.
		/// </summary>
		/// <returns>True if a gizmo gets changed, otherwise false.</returns>
		virtual bool RenderGizmos(const ImRect& a_SceneViewRect)
		{
			return false;
		}

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override
		{}

		/// <summary>
		/// Called when the selectable gets renamed.
		/// </summary>
		/// <param name="a_sDisplayName">The new name.</param>
		virtual void OnRename(const std::string& a_sName)
		{}

		/// <summary>
		/// Called when the selectable gets deleted.
		/// </summary>
		virtual void OnDelete()
		{}

		/// <summary>
		/// Called when the selectable's folder gets called.
		/// </summary>
		virtual void OnShowInExplorer()
		{}

		/// <summary>
		/// Returns the name of the selectable.
		/// </summary>
		/// <returns>String containing the name.</returns>
		virtual const std::string& GetName() const = 0;

		/// <summary>
		/// Called when the selectable gets double clicked.
		/// </summary>
		virtual void OnDoubleClicked()
		{}

		/// <summary>
		/// Returns the icon of the selectable.
		/// </summary>
		/// <returns>String containing the icon.</returns>
		virtual const std::string& GetIcon() const = 0;

		/// <summary>
		/// Retrieves if a selectable can be renamed.
		/// </summary>
		/// <returns>True if the selectable can get renamed, otherwise false.</returns>
		bool GetShowRename() const
		{
			return m_bShowRename;
		}

		/// <summary>
		/// Sets whether a selectable can be renamed.
		/// </summary>
		/// <param name="a_bShowRename">True if the selectable can be renamed, false otherwise.</param>
		void SetShowRename(bool a_bShowRename)
		{
			m_bShowRename = a_bShowRename;
		}

		/// <summary>
		/// Retrieves if a selectable can be deleted.
		/// </summary>
		/// <returns>True if the selectable can get deleted, otherwise false.</returns>
		bool GetShowDelete() const
		{
			return m_bShowDelete;
		}

		/// <summary>
		/// Sets whether a selectable can be deleted.
		/// </summary>
		/// <param name="a_bShowDelete">True if the selectable can be deleted, false otherwise.</param>
		void SetShowDelete(bool a_bShowDelete)
		{
			m_bShowDelete = a_bShowDelete;
		}

		/// <summary>
		/// Retrieves if a selectable can be opened in the explorer.
		/// </summary>
		/// <returns>True if the selectable can be opened in the explorer, otherwise false.</returns>
		bool GetShowShowInExplorer() const
		{
			return m_bShowShowInExplorer;
		}

		/// <summary>
		/// Sets whether a selectable can be shown in explorer.
		/// </summary>
		/// <param name="a_bShowShowInExplorer">True if the selectable can be shown in explorer, false otherwise.</param>
		void SetShowShowInExplorer(bool a_bShowShowInExplorer)
		{
			m_bShowShowInExplorer = a_bShowShowInExplorer;
		}

		/// <summary>
		/// Called when the selectable gets selected.
		/// </summary>
		virtual void OnSelect()
		{}

		/// <summary>
		/// Called when the selectable gets deselected.
		/// </summary>
		virtual void OnDeselect()
		{}
	protected:
		bool
			m_bShowRename = false,
			m_bShowDelete = false,
			m_bShowShowInExplorer = false;
	};
}