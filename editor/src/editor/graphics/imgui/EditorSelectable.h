#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
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

				virtual bool RenderGizmos(const ImRect& a_SceneViewRect)
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