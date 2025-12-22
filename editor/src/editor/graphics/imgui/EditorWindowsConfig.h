#pragma once

#include "graphics/imgui/ImGuiWindowsConfig.h"

// editor
#include "editor/graphics/imgui/windows/EditorWindowDock.h"
#include "editor/graphics/imgui/windows/ResourcesWindow.h"
#include "editor/graphics/imgui/windows/ConsoleWindow.h"
#include "editor/graphics/imgui/windows/HierarchyWindow.h"
#include "editor/graphics/imgui/windows/SceneWindow.h"
#include "editor/graphics/imgui/windows/ExplorerWindow.h"
#include "editor/graphics/imgui/windows/InspectorWindow.h"
#include "editor/graphics/imgui/windows/AnimationWindow.h"
#include "editor/graphics/imgui/windows/StatsWindow.h"

#include "editor/graphics/imgui/modals/FilePickerModal.h"

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

				EditorWindowDock& GetEditorWindowDock()
				{
					return m_EditorWindowDock;
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

				AnimationWindow& GetAnimationWindow()
				{
					return m_AnimationWindow;
				}

				StatsWindow& GetStatsWindow()
				{
					return m_StatsWindow;
				}

				FilePickerModal& GetFilePickerModal()
				{
					return m_FilePickerModal;
				}
			private:
				EditorWindowDock m_EditorWindowDock;

				ResourcesWindow m_ResourcesWindow;
				ConsoleWindow m_ConsoleWindow;
				HierarchyWindow m_HierarchyWindow;
				SceneWindow m_SceneWindow;
				FullSceneWindow m_FullSceneWindow;
				ExplorerWindow m_ExplorerWindow;
				InspectorWindow m_InspectorWindow;
				AnimationWindow m_AnimationWindow;
				StatsWindow m_StatsWindow;

				FilePickerModal m_FilePickerModal;
			};
		}
	}
}