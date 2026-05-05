#pragma once

#include "imgui_system/windows/MainWindowDock.h"

// external
#include <vector>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiSystem;

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
				/// <param name="a_Window">The ImGui system for rendering the view.</param>
				EditorWindowDock(ImGuiSystem& a_System);

				/// <summary>
				/// Renders the element.
				/// </summary>
				void Render() override;
			};
		}
	}
}