#pragma once

#include "imgui_system/ImGuiWindowsConfig.h"

namespace gallus::graphics::imgui
{
	class EditorWindowDock;

	class ConsoleWindow;
	class ExplorerWindow;
	class InspectorWindow;
	class AnimationWindow;
	class HierarchyWindow;
	class SceneWindow;
	class FullSceneWindow;
	class SpriteSheetEditorWindow;

	class FilePickerModal;
	class PromptModal;

	class ImGuiSystem;

	class EditorWindowsConfig : public ImGuiWindowsConfig
	{
	public:
		EditorWindowsConfig(ImGuiSystem& a_System);

		/// <summary>
		/// Initializes the windows config.
		/// </summary>
		void Initialize() override;

		/// <summary>
		/// Renders all windows, modals and docks in the config.
		/// </summary>
		void Render() override;

		/// <summary>
		/// Retrieves the editor window dock.
		/// </summary>
		/// <returns>Pointer to the editor window dock.</returns>
		EditorWindowDock* GetEditorWindowDock()
		{
			return m_EditorWindowDock;
		}

		/// <summary>
		/// Retrieves the console window.
		/// </summary>
		/// <returns>Pointer to the console window.</returns>
		ConsoleWindow* GetConsoleWindow()
		{
			return m_ConsoleWindow;
		}

		/// <summary>
		/// Retrieves the explorer window.
		/// </summary>
		/// <returns>Pointer to the explorer window.</returns>
		ExplorerWindow* GetExplorerWindow()
		{
			return m_ExplorerWindow;
		}

		/// <summary>
		/// Retrieves the hierarchy window.
		/// </summary>
		/// <returns>Pointer to the hierarchy window.</returns>
		HierarchyWindow* GetHierarchyWindow()
		{
			return m_HierarchyWindow;
		}

		/// <summary>
		/// Retrieves the animation window.
		/// </summary>
		/// <returns>Pointer to the animation window.</returns>
		AnimationWindow* GetAnimationWindow()
		{
			return m_AnimationWindow;
		}

		/// <summary>
		/// Retrieves the inspector window.
		/// </summary>
		/// <returns>Pointer to the inspector window.</returns>
		InspectorWindow* GetInspectorWindow()
		{
			return m_InspectorWindow;
		}

		/// <summary>
		/// Retrieves the scene window.
		/// </summary>
		/// <returns>Pointer to the scene window.</returns>
		SceneWindow* GetSceneWindow()
		{
			return m_SceneWindow;
		}

		/// <summary>
		/// Retrieves the full scene window.
		/// </summary>
		/// <returns>Pointer to the full scene window.</returns>
		FullSceneWindow* GetFullSceneWindow()
		{
			return m_FullSceneWindow;
		}

		/// <summary>
		/// Retrieves the sprite sheet editor window.
		/// </summary>
		/// <returns>Pointer to the sprite sheet editor window.</returns>
		SpriteSheetEditorWindow* GetSpriteSheetEditorWindow()
		{
			return m_SpriteSheetEditorWindow;
		}

		/// <summary>
		/// Retrieves the file picker modal.
		/// </summary>
		/// <returns>Pointer to the file picker modal.</returns>
		FilePickerModal* GetFilePickerModal()
		{
			return m_FilePickerModal;
		}

		/// <summary>
		/// Retrieves the prompt modal.
		/// </summary>
		/// <returns>Pointer to the prompt modal.</returns>
		PromptModal* GetPromptModal()
		{
			return m_PromptModal;
		}
	private:
		ImGuiSystem& m_ImGuiSystem;

		EditorWindowDock* m_EditorWindowDock = nullptr;

		ConsoleWindow* m_ConsoleWindow = nullptr;
		ExplorerWindow* m_ExplorerWindow = nullptr;
		HierarchyWindow* m_HierarchyWindow = nullptr;
		AnimationWindow* m_AnimationWindow = nullptr;
		InspectorWindow* m_InspectorWindow = nullptr;
		SceneWindow* m_SceneWindow = nullptr;
		FullSceneWindow* m_FullSceneWindow = nullptr;
		SpriteSheetEditorWindow* m_SpriteSheetEditorWindow = nullptr;

		FilePickerModal* m_FilePickerModal = nullptr;
		PromptModal* m_PromptModal = nullptr;
	};
}