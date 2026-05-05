#include "MainWindowPanel.h"

// external
#include <imgui/imgui.h>

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// MainWindowPanel
	//---------------------------------------------------------------------
	MainWindowPanel::MainWindowPanel(ImGuiSystem& a_System) : BaseWindow(a_System, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar, "", "Main Window", true)
	{}
}