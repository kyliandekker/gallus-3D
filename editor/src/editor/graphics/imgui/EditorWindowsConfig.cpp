#include "EditorWindowsConfig.h"

// imgui
#include "imgui_system/ImGuiSystem.h"

// editor
#include "editor/graphics/imgui/windows/EditorWindowDock.h"
#include "editor/graphics/imgui/windows/ConsoleWindow.h"
#include "editor/graphics/imgui/windows/ExplorerWindow.h"
#include "editor/graphics/imgui/windows/HierarchyWindow.h"
#include "editor/graphics/imgui/windows/AnimationWindow.h"
#include "editor/graphics/imgui/windows/InspectorWindow.h"
#include "editor/graphics/imgui/windows/SceneWindow.h"

#include "editor/graphics/imgui/modals/FilePickerModal.h"
#include "editor/graphics/imgui/modals/PromptModal.h"
#include "editor/graphics/imgui/windows/SpriteSheetEditorWindow.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// EditorWindowsConfig
	//---------------------------------------------------------------------
	EditorWindowsConfig::EditorWindowsConfig(ImGuiSystem& a_System) : m_ImGuiSystem(a_System)
	{
		m_EditorWindowDock = new EditorWindowDock(a_System);

		m_ConsoleWindow = new ConsoleWindow(a_System);
		m_ExplorerWindow = new ExplorerWindow(a_System);
		m_HierarchyWindow = new HierarchyWindow(a_System);
		m_AnimationWindow = new AnimationWindow(a_System);
		m_InspectorWindow = new InspectorWindow(a_System);
		m_SceneWindow = new SceneWindow(a_System);
		m_FullSceneWindow = new FullSceneWindow(a_System);
		m_SpriteSheetEditorWindow = new SpriteSheetEditorWindow(a_System);

		m_FilePickerModal = new FilePickerModal(a_System);
		m_PromptModal = new PromptModal(a_System);
	}

	//---------------------------------------------------------------------
	void EditorWindowsConfig::Initialize()
	{
		m_EditorWindowDock->Initialize();

		m_ConsoleWindow->Initialize();
		m_ExplorerWindow->Initialize();
		m_HierarchyWindow->Initialize();
		m_AnimationWindow->Initialize();
		m_InspectorWindow->Initialize();
		m_SceneWindow->Initialize();
		m_FullSceneWindow->Initialize();
		m_SpriteSheetEditorWindow->Initialize();

		m_FilePickerModal->Initialize();
		m_PromptModal->Initialize();
	}

	//---------------------------------------------------------------------
	void EditorWindowsConfig::Render()
	{
		m_EditorWindowDock->Update();

		m_ConsoleWindow->Update();
		m_ExplorerWindow->Update();
		m_HierarchyWindow->Update();
		m_AnimationWindow->Update();
		m_InspectorWindow->Update();
		m_SceneWindow->Update();
		m_FullSceneWindow->Update();
		m_SpriteSheetEditorWindow->Update();

		m_FilePickerModal->Update();
		m_PromptModal->Update();
	}
}