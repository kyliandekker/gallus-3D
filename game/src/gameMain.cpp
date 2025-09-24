#include <windows.h>
#include <Shlwapi.h>
#include <filesystem>
#include <ShellScalingApi.h>

// core includes
#include "resource.h"
#include "core/ArgProcessor.h"

// logger includes
#include "logger/Logger.h"

// utils includes
#include "utils/file_abstractions.h"

// gameplay includes
#include "gameplay/Game.h"

// editor includes
#include "core/Engine.h"

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
	std::string name = "Game";
	std::string saveDirPath = gallus::file::GetAppDataPath().generic_string() + "/game";
	gallus::core::ENGINE = new gallus::core::Engine();
	gallus::core::ENGINE->SetSaveDirectory(saveDirPath);
	gallus::core::ENGINE->SetDefaultArguments();

	gallus::core::ARGS.ProcessArguments(cmdLine);

	gallus::core::ENGINE->Initialize(hInstance, name);

	// Load icons.
	HICON hIconLarge = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	HICON hIconSmall = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0); // 16x16
	SendMessage(gallus::core::ENGINE->GetWindow().GetHWnd(), WM_SETICON, ICON_BIG, (LPARAM) hIconLarge);
	SendMessage(gallus::core::ENGINE->GetWindow().GetHWnd(), WM_SETICON, ICON_SMALL, (LPARAM) hIconSmall);

	// Game
	gallus::gameplay::GAME.Initialize();

	gallus::gameplay::GAME.Loop();

	// Destroy the tool after loop ends.
	gallus::core::ENGINE->Destroy();

	return 0;
}