#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>
#include <filesystem>
#include <ShellScalingApi.h>

// core
#include "resource.h"
#include "core/ArgProcessor.h"

// graphics
#include "graphics/dx12/DX12System.h"

#include "graphics/win32/Window.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"

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

	// Initialize systems.
	std::string name = "Gallus Engine";
	std::string saveDirPath = gallus::file::GetAppDataPath().generic_string() + "/engine";
	gallus::core::EDITOR_ENGINE = new gallus::core::EditorEngine();
	gallus::core::ENGINE = gallus::core::EDITOR_ENGINE;
	gallus::core::EDITOR_ENGINE->SetSaveDirectory(saveDirPath);
	gallus::core::EDITOR_ENGINE->SetDefaultArguments();

	gallus::core::ARGS.ProcessArguments(cmdLine);

	gallus::graphics::dx12::DX12System* dx12System = gallus::core::ENGINE->GetDX12();
	if (!dx12System)
	{
		return 0;
	}

	gallus::graphics::imgui::ImGuiWindow& imguiWindow = dx12System->GetImGuiWindow();
	imguiWindow.SetWindowsConfig<gallus::graphics::imgui::EditorWindowsConfig>();

	gallus::core::EDITOR_ENGINE->Initialize(hInstance, name);

	gallus::graphics::win32::Window* window = gallus::core::ENGINE->GetWindow();
	if (!window)
	{
		return 0;
	}

	// Load icons.
	HICON hIconLarge = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	HICON hIconSmall = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0); // 16x16
	SendMessage(window->GetHWnd(), WM_SETICON, ICON_BIG, (LPARAM) hIconLarge);
	SendMessage(window->GetHWnd(), WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);

	// Game
	gallus::gameplay::GAME.Initialize();

	imguiWindow.InitializeWindows();

	gallus::gameplay::GAME.Loop();

	// Destroy the tool after loop ends.
	gallus::core::EDITOR_ENGINE->Destroy();

	return 0;
}