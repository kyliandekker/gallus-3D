#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;
			class ExplorerFileUIView;

			/// <summary>
			/// Base class for rendering files in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with files.
			/// </summary>
			class ExplorerFileUIViewInfo : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs the ExplorerFileUIViewInfo with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ExplorerFileUIView">The explorer file ui view.</param>
				ExplorerFileUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ImGuiUIView(a_Window), m_ExplorerFileUIView(a_ExplorerFileUIView)
				{}

				virtual ~ExplorerFileUIViewInfo() = default;

				/// <summary>
				/// Renders the file UI in the editor.
				/// </summary>
				void Render();

				/// <summary>
				/// Renders the file UI in the editor preview section.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				virtual void RenderPreview()
				{}

				bool GetShowPreview() const
				{
					return m_bShowPreview;
				}
			protected:
				/// <summary>
				/// Renders the file UI in the editor.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				virtual void RenderSpecific()
				{}

				bool m_bShowPreview = false;
				ExplorerFileUIView& m_ExplorerFileUIView;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE