#include <windows.h>
#include <Shlwapi.h>
#include <filesystem>
#include <ShellScalingApi.h>

#include "utils/file_abstractions.h"
#include "core/EditorTool.h"
#include "gameplay/Game.h"
#include "resource.h"

#include "graphics/imgui/windows/MainWindowDock.h"
#include "graphics/imgui/windows/ConsoleWindow.h"
#include "graphics/imgui/windows/HierarchyWindow.h"
#include "graphics/imgui/windows/SceneWindow.h"
#include "graphics/imgui/windows/ExplorerWindow.h"
#include "graphics/imgui/windows/InspectorWindow.h"
#include "graphics/imgui/modals/FilePickerModal.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);

	fs::path path(buffer);
	path = path.parent_path();
	fs::current_path(path);
	SetCurrentDirectoryW(path.c_str());

	// Initialize systems.
	std::string name = "Gallus 2D Engine";
	std::string saveDirPath = gallus::file::GetAppDataPath().generic_string() + "/tool";
	std::string assetPath = "./data/assets/";
	gallus::core::TOOL = new gallus::core::EditorTool();
	gallus::core::TOOL->SetSaveDirectory(saveDirPath);
	gallus::core::TOOL->GetResourceAtlas().SetResourceFolder(assetPath);

	gallus::graphics::imgui::ImGuiWindow& imguiWindow = gallus::core::TOOL->GetDX12().GetImGuiWindow();
	imguiWindow.AddWindow(new gallus::graphics::imgui::MainWindowDock(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::editor::ConsoleWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::editor::HierarchyWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::editor::SceneWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::editor::ExplorerWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::editor::InspectorWindow(imguiWindow));
	auto* t = new gallus::graphics::imgui::editor::FilePickerModal(imguiWindow);
	imguiWindow.AddModal(t);

	gallus::core::TOOL->Initialize(hInstance, name);

	imguiWindow.InitializeWindows();

	// Load icons.
	HICON hIconLarge = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	HICON hIconSmall = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0); // 16x16
	SendMessage(gallus::core::TOOL->GetWindow().GetHWnd(), WM_SETICON, ICON_BIG, (LPARAM) hIconLarge);
	SendMessage(gallus::core::TOOL->GetWindow().GetHWnd(), WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);

	// Game
	game::GAME.Initialize();
	game::GAME.Loop();

	// Destroy the tool after loop ends.
	gallus::core::TOOL->Destroy();

	return 0;
}