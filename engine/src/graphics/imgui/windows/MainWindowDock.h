#ifndef IMGUI_DISABLE

#pragma once

#include "BaseWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// MainWindowDock
			//---------------------------------------------------------------------
			/// <summary>
			/// Main window that allows docking for other windows.
			/// </summary>
			class MainWindowDock : public BaseWindow
			{
			public:
				/// <summary>
				/// Constructs the main window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				MainWindowDock(ImGuiWindow& a_Window);

				/// <summary>
				/// Renders the element.
				/// </summary>
				void Render() override;
			};
		}
	}
}

#endif // IMGUI_DISABLE