#include "EditorWindowDock.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

// gameplay
#include "gameplay/Game.h"

// utils
#include "file/file_abstractions.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/Editor.h"
#include "editor/AssetDatabase.h"

#include "editor/GlobalEditorFunctions.h"

// editor/file
#include "editor/file/FileResource.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			EditorWindowDock::EditorWindowDock(ImGuiSystem& a_System) : MainWindowDock(a_System)
			{}

			void EditorWindowDock::Render()
			{
				editor::AssetDatabase* assetDatabase = GetEditorEngine().GetAssetDatabase();
				if (!assetDatabase)
				{
					return;
				}
				
				editor::Editor* editor = GetEditorEngine().GetEditor();
				if (!editor)
				{
					return;
				}

				ImGui::BeginMainMenuBar();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_System.GetFramePadding() * 2);
				if (ImGui::BeginMenu(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE + std::string(" File"), MENU_ID, "FILE_EDITOR").c_str()))
				{
					if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_SCENE + std::string(" New Scene"), MENU_ITEM_ID, "NEW_SCENE_EDITOR").c_str(), "CTRL+N"))
					{
						gameplay::GetGame().GetScene().Destroy();
						gameplay::GetGame().GetScene().LoadData();
						gameplay::GetGame().GetScene().SetName("");

						editor->GetScene().Destroy();
						editor->GetScene().LoadData();
					}
					if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" Open Scene"), MENU_ITEM_ID, "OPEN_SCENE_EDITOR").c_str(), "CTRL+O"))
					{
						fs::path scenePath;
						if (file::PickFile(scenePath, {
								{ L"Scene Files (*.scene)", L"*.scene" },
							}, assetDatabase->GetResourceFolder()->GetPath()))
						{
							core::Data data;
							if (file::LoadFile(scenePath, data))
							{
								std::string assetId = scenePath.filename().generic_string();

								if (scenePath.extension() == ".scene")
								{
									editor::g_TrySetEditorScene(assetId);
								}
							}
						}
					}
					if (ImGui::BeginMenu(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" Open Recent Scene"), MENU_ID, "OPEN_RECENT_SCENE_EDITOR").c_str()))
					{
						for (const std::string& scene : editor->GetEditorSettings().GetLastOpenedScenes())
						{
							if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" ") + scene, MENU_ITEM_ID, scene + "_EDITOR").c_str(), "CTRL+S"))
							{
								editor::g_TrySetEditorScene(scene);
							}
						}
						ImGui::EndMenu();
					}
					ImGui::Separator();
					bool wasDirty = editor::g_IsEditorSceneDirty() && !gameplay::GetGame().IsStarted();
					if (!wasDirty)
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}

					if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_SAVE + std::string(" Save Scene"), MENU_ITEM_ID, "SAVE_SCENE_EDITOR").c_str(), "CTRL+S"))
					{
						editor::g_SaveEditorScene();
					}

					if (!wasDirty)
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					ImGui::EndMenu();
				}
				ImGui::PopStyleVar();
				ImGui::EndMainMenuBar();
				MainWindowDock::Render();
			}
		}
	}
}