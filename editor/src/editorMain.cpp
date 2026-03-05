#include <Shlwapi.h>
#include <filesystem>
#include <ShellScalingApi.h>

// core
#include "resource.h"
#include "core/ArgProcessor.h"

// graphics
#include "graphics/win32/WINPCH.h"
#include "graphics/win32/Window.h"

// gameplay
#include "gameplay/Game.h"

// resources
#include "resources/ResourceAtlas.h"

// file
#include "file/file_abstractions.h"

// imgui
#include "imgui_system/ImGuiSystem.h"

// editor
#include "editor/editorEntryPoints.h"

#include "editor/Editor.h"
#include "editor/core/EditorEngine.h"

// editor/file
#include "editor/file/IFileAssetSource.h"

//---------------------------------------------------------------------
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
	SetEnginePtr(new gallus::core::EditorEngine());
	gallus::GetEditorEngine().SetDefaultArguments();

	gallus::core::ARGS.ProcessArguments(cmdLine);
	gallus::GetEditorEngine().GetResourceAtlas()->SetAssetSource<gallus::file::IFileAssetSource>();
	gallus::GetEditorEngine().GetWindow()->GetWindowSettings().SetPath(saveDirPath);
	gallus::GetEditorEngine().GetEditor()->GetEditorSettings().SetPath(saveDirPath);
	gallus::GetEditorEngine().GetImGuiSystem()->SetIniPath(saveDirPath);
	gallus::GetEditorEngine().Initialize(hInstance, name);

	gallus::graphics::win32::Window* window = gallus::GetEditorEngine().GetWindow();
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
	gallus::gameplay::GetGame().Initialize();

	gallus::gameplay::GetGame().Loop();

	// Destroy the tool after loop ends.
	gallus::GetEngine().Destroy();

	return 0;
}