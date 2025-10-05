#ifndef IMGUI_DISABLE

#include "MainWindowDock.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// MainWindowDock
			//---------------------------------------------------------------------
			MainWindowDock::MainWindowDock(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking, "", "DockSpace", true)
			{}

			//---------------------------------------------------------------------
			void MainWindowDock::Render()
			{
				ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(), ImGuiDockNodeFlags_PassthruCentralNode);
			}
		}
	}
}

#endif // IMGUI_DISABLE