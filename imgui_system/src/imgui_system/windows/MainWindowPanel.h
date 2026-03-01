#pragma once

#include "BaseWindow.h"

namespace gallus::graphics::imgui
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
		/// <param name="a_System">The ImGui System for rendering the view.</param>
		MainWindowPanel(ImGuiSystem& a_System);

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override = 0;
	};
}