#ifndef IMGUI_DISABLE

#include "MainWindowPanel.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_helpers.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// MainWindowPanel
			//---------------------------------------------------------------------
			MainWindowPanel::MainWindowPanel(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar, "", "Main Window", true)
			{}
		}
	}
}

#endif // IMGUI_DISABLE