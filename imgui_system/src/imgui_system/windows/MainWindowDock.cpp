#include "MainWindowDock.h"

// external
#include <imgui/imgui.h>

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// MainWindowDock
	//---------------------------------------------------------------------
	MainWindowDock::MainWindowDock(ImGuiSystem& a_System) : BaseWindow(a_System, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar, "", "DockSpace", true)
	{}

	//---------------------------------------------------------------------
	void MainWindowDock::Render()
	{
		ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(), ImGuiDockNodeFlags_PassthruCentralNode);
	}
}