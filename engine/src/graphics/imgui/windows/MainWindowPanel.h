#ifndef IMGUI_DISABLE

#pragma once

// base class
#include "BaseWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// MainWindowPanel
			//---------------------------------------------------------------------
			/// <summary>
			/// Main window that just is one window (no docking).
			/// </summary>
			class MainWindowPanel : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs the main window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				MainWindowPanel(ImGuiWindow& a_Window);

				void Render() override = 0;
			};
		}
	}
}

#endif // IMGUI_DISABLE