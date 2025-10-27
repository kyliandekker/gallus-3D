#include "EditorWindowsConfig.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			EditorWindowsConfig::EditorWindowsConfig(ImGuiWindow& a_Window) :
				m_MainWindowDock(a_Window),
				m_ResourcesWindow(a_Window),
				m_ConsoleWindow(a_Window),
				m_HierarchyWindow(a_Window),
				m_SceneWindow(a_Window),
				m_FullSceneWindow(a_Window),
				m_ExplorerWindow(a_Window),
				m_InspectorWindow(a_Window),
				m_FilePickerModal(a_Window),
				m_SpriteEditorModal(a_Window),
				m_AnimationEditorModal(a_Window)
			{
			}

			void EditorWindowsConfig::Initialize()
			{
				m_MainWindowDock.Initialize();

				m_ResourcesWindow.Initialize();
				m_ConsoleWindow.Initialize();
				m_HierarchyWindow.Initialize();
				m_SceneWindow.Initialize();
				m_FullSceneWindow.Initialize();
				m_ExplorerWindow.Initialize();
				m_InspectorWindow.Initialize();

				m_FilePickerModal.Initialize();
				m_SpriteEditorModal.Initialize();
				m_AnimationEditorModal.Initialize();
			}

			void EditorWindowsConfig::Render()
			{
				m_MainWindowDock.Update();

				m_ResourcesWindow.Update();
				m_ConsoleWindow.Update();
				m_HierarchyWindow.Update();
				m_SceneWindow.Update();
				m_FullSceneWindow.Update();
				m_ExplorerWindow.Update();
				m_InspectorWindow.Update();

				m_FilePickerModal.Update();
				m_SpriteEditorModal.Update();
				m_AnimationEditorModal.Update();
			}
		}
	}
}