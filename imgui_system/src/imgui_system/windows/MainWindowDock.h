#pragma once

#include "BaseWindow.h"

namespace gallus::graphics::imgui
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
		/// <param name="a_System">The ImGui System for rendering the view.</param>
		MainWindowDock(ImGuiSystem& a_System);

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override;
	};
}