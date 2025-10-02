#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// base class
#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <string>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Abstract class for items that will be viewable in the inspector.
			/// </summary>
			class InspectorUIView : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs an inspector view.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				InspectorUIView(ImGuiWindow& a_Window) : ImGuiUIView(a_Window)
				{}

				virtual ~InspectorUIView() = default;

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

				virtual std::string GetIcon() const
				{
					return "";
				}

				virtual void RenderPreview()
				{}

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

				bool GetShowPreview() const
				{
					return m_bShowPreview;
				}
			protected:
				bool
					m_bShowRename = false,
					m_bShowDelete = false,
					m_bShowShowInExplorer = false,
					m_bShowPreview = false;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE