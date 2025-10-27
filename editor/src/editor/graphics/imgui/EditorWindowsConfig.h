#pragma once

#include "graphics/imgui/ImGuiWindowsConfig.h"

#include "graphics/imgui/windows/MainWindowDock.h"
#include "editor/graphics/imgui/windows/ResourcesWindow.h"
#include "editor/graphics/imgui/windows/ConsoleWindow.h"
#include "editor/graphics/imgui/windows/HierarchyWindow.h"
#include "editor/graphics/imgui/windows/SceneWindow.h"
#include "editor/graphics/imgui/windows/ExplorerWindow.h"
#include "editor/graphics/imgui/windows/InspectorWindow.h"

#include "editor/graphics/imgui/modals/FilePickerModal.h"
#include "editor/graphics/imgui/modals/SpriteEditorModal.h"
#include "editor/graphics/imgui/modals/AnimationEditorModal.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

			class EditorWindowsConfig : public ImGuiWindowsConfig
			{
			public:
				EditorWindowsConfig(ImGuiWindow& a_Window);

				void Initialize() override;
				void Render() override;

				MainWindowDock& GetMainWindowDock()
				{
					return m_MainWindowDock;
				}

				ResourcesWindow& GetResourcesWindow()
				{
					return m_ResourcesWindow;
				}

				ConsoleWindow& GetConsoleWindow()
				{
					return m_ConsoleWindow;
				}

				HierarchyWindow& GetHierarchyWindow()
				{
					return m_HierarchyWindow;
				}

				SceneWindow& GetSceneWindow()
				{
					return m_SceneWindow;
				}

				FullSceneWindow& GetFullSceneWindow()
				{
					return m_FullSceneWindow;
				}

				ExplorerWindow& GetExplorerWindow()
				{
					return m_ExplorerWindow;
				}

				InspectorWindow& GetInspectorWindow()
				{
					return m_InspectorWindow;
				}

				FilePickerModal& GetFilePickerModal()
				{
					return m_FilePickerModal;
				}

				SpriteEditorModal& GetSpriteEditorModal()
				{
					return m_SpriteEditorModal;
				}

				AnimationEditorModal& GetAnimationEditorModal()
				{
					return m_AnimationEditorModal;
				}
			private:
				MainWindowDock m_MainWindowDock;

				ResourcesWindow m_ResourcesWindow;
				ConsoleWindow m_ConsoleWindow;
				HierarchyWindow m_HierarchyWindow;
				SceneWindow m_SceneWindow;
				FullSceneWindow m_FullSceneWindow;
				ExplorerWindow m_ExplorerWindow;
				InspectorWindow m_InspectorWindow;

				FilePickerModal m_FilePickerModal;
				SpriteEditorModal m_SpriteEditorModal;
				AnimationEditorModal m_AnimationEditorModal;
			};
		}
	}
}