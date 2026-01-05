#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <imgui/imgui.h>
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class EditorSelectable : public ImGuiUIView
			{
			public:
				EditorSelectable(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				virtual bool RenderEditorFields()
				{
					return false;
				}

				virtual bool RenderGizmos(
					const ImVec2& a_vScenePos,
					const ImVec2& a_vSize,
					const ImVec2& a_vPanOffset,
					float a_fZoom)
				{
					return false;
				}

				void Render() override
				{}

				virtual void OnRename(const std::string& a_sName)
				{}

				virtual void OnDelete()
				{}

				virtual void OnShowInExplorer()
				{}

				virtual std::string GetName() const
				{
					return "";
				}

				virtual void OnDoubleClicked()
				{}

				virtual std::string GetIcon() const
				{
					return "";
				}

				bool GetShowRename() const
				{
					return m_bShowRename;
				}

				bool GetShowDelete() const
				{
					return m_bShowDelete;
				}

				bool GetShowShowInExplorer() const
				{
					return m_bShowShowInExplorer;
				}

				virtual void Select()
				{}

				virtual void Deselect()
				{}
			protected:
				bool
					m_bShowRename = false,
					m_bShowDelete = false,
					m_bShowShowInExplorer = false;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE