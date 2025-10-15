#ifndef IMGUI_DISABLE

#pragma once

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			/// <summary>
			/// Generic UI view class used by all elements in the editor.
			/// </summary>
			class ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a ui view for a given window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ImGuiUIView(ImGuiWindow& a_Window) : m_Window(a_Window)
				{}

				virtual ~ImGuiUIView() = default;

				/// <summary>
				/// Renders the element.
				/// </summary>
				virtual void Render() = 0;

				/// <summary>
				/// Retrieves the window the element is being rendered to.
				/// </summary>
				/// <returns>Reference to the window.</returns>
				ImGuiWindow& Window() const
				{
					return m_Window;
				}
			protected:
				ImGuiWindow& m_Window; // The window the element is being rendered to.
			};
		}
	}
}

#endif // IMGUI_DISABLE