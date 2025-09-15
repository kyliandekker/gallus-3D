#include <windows.h>
#include <Shlwapi.h>
#include <filesystem>
#include <ShellScalingApi.h>

// core includes
#include "resource.h"

// utils includes
#include "utils/file_abstractions.h"

// graphics includes
#include "graphics/imgui/windows/MainWindowDock.h"

// gameplay includes
#include "gameplay/Game.h"

// editor includes
#include "editor/core/EditorTool.h"
#include "editor/graphics/imgui/windows/ConsoleWindow.h"
#include "editor/graphics/imgui/windows/HierarchyWindow.h"
#include "editor/graphics/imgui/windows/SceneWindow.h"
#include "editor/graphics/imgui/windows/ExplorerWindow.h"
#include "editor/graphics/imgui/windows/InspectorWindow.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"
#include "editor/graphics/imgui/modals/SpriteEditorModal.h"

std::string getCommandArg(const std::string& args, const std::string& argName, const std::string& defaultVal)
{
	std::string val = defaultVal;

	std::string search = argName + "=";
	size_t pos = args.find(search);
	if (pos != std::string::npos)
	{
		size_t start = pos + search.size();

		if (args[start] == '"')
		{
			size_t end = args.find('"', start + 1);
			if (end != std::string::npos)
			{
				val = args.substr(start + 1, end - start - 1);
			}
		}
		else
		{
			size_t end = args.find(' ', start);
			if (end == std::string::npos)
				val = args.substr(start);
			else
				val = args.substr(start, end - start);
		}
	}

	return val;
}

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

	std::wstring wstr = lpCmdLine;
	std::string cmdLine(wstr.begin(), wstr.end());

	std::string assetPath = getCommandArg(cmdLine, "assetPath", "./data/assets/");

	// Initialize systems.
	std::string name = "Gallus 2D Engine";
	std::string saveDirPath = gallus::file::GetAppDataPath().generic_string() + "/tool";
	gallus::core::TOOL = new gallus::core::EditorTool();
	gallus::core::TOOL->SetSaveDirectory(saveDirPath);
	gallus::core::TOOL->GetResourceAtlas().SetResourceFolder(assetPath);

	gallus::graphics::imgui::ImGuiWindow& imguiWindow = gallus::core::TOOL->GetDX12().GetImGuiWindow();
	imguiWindow.AddWindow(new gallus::graphics::imgui::MainWindowDock(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::ConsoleWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::HierarchyWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::SceneWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::ExplorerWindow(imguiWindow));
	imguiWindow.AddWindow(new gallus::graphics::imgui::InspectorWindow(imguiWindow));
	imguiWindow.AddModal(new gallus::graphics::imgui::FilePickerModal(imguiWindow));
	imguiWindow.AddModal(new gallus::graphics::imgui::SpriteEditorModal(imguiWindow));

	gallus::core::TOOL->Initialize(hInstance, name);

	imguiWindow.InitializeWindows();

	// Load icons.
	HICON hIconLarge = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	HICON hIconSmall = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0); // 16x16
	SendMessage(gallus::core::TOOL->GetWindow().GetHWnd(), WM_SETICON, ICON_BIG, (LPARAM) hIconLarge);
	SendMessage(gallus::core::TOOL->GetWindow().GetHWnd(), WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);

	// Game
	gallus::gameplay::GAME.Initialize();
	gallus::gameplay::GAME.Loop();

	// Destroy the tool after loop ends.
	gallus::core::TOOL->Destroy();

	return 0;
}