#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/MainWindowDock.h"

// external
#include <vector>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			//---------------------------------------------------------------------
			// StatsWindow
			//---------------------------------------------------------------------
			/// <summary>
			/// Main window that allows docking.
			/// </summary>
			class EditorWindowDock : public MainWindowDock
			{
			public:
				/// <summary>
				/// Constructs the main window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				EditorWindowDock(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the element.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE