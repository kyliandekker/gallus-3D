#include "EditorWindowDock.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/imgui/font_icon.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/selectables/EntityEditorSelectable.h"
#include "editor/graphics/imgui/selectables/FileEditorSelectable.h"

#include "editor/EditorGlobalFunctions.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			EditorWindowDock::EditorWindowDock(ImGuiWindow& a_Window) : MainWindowDock(a_Window)
			{}

			void EditorWindowDock::Render()
			{
				ImGui::BeginMainMenuBar();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Window.GetFramePadding() * 2);
				if (ImGui::BeginMenu(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE + std::string(" File"), MENU_ID, "FILE_EDITOR").c_str()))
				{
					if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_SCENE + std::string(" New Scene"), MENU_ITEM_ID, "NEW_SCENE_EDITOR").c_str(), "CTRL+N"))
					{
						gameplay::GAME.GetScene().Destroy();
						gameplay::GAME.GetScene().LoadData();

						core::EDITOR_ENGINE->GetEditor().GetScene().Destroy();
						core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
					}
					if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" Open Scene"), MENU_ITEM_ID, "OPEN_SCENE_EDITOR").c_str(), "CTRL+O"))
					{
						fs::path scenePath;
						if (file::PickFile(scenePath, {
							{ L"Scene Files (*.scene)", L"*.scene" },
							{ L"Prefab Files (*.prefab)", L"*.prefab" },
							}, core::EDITOR_ENGINE->GetResourceAtlas().GetResourceFolder().GetPath()))
						{
							core::Data data;
							if (file::LoadFile(scenePath, data))
							{
								if (scenePath.extension() == ".scene")
								{
									editor::g_SetScene(scenePath, editor::EditorMethod::EDITOR_METHOD_SCENE);
								}
								else if (scenePath.extension() == ".prefab")
								{
									editor::g_SetScene(scenePath, editor::EditorMethod::EDITOR_METHOD_PREFAB);
								}

								core::EDITOR_ENGINE->GetEditor().GetScene().LoadByPath(scenePath);
								core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
							}
						}
					}
					ImGui::Separator();
					bool wasDirty = core::EDITOR_ENGINE->GetEditor().GetScene().IsDirty() && !gameplay::GAME.IsStarted();
					if (!wasDirty)
					{
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
					}

					if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_SAVE + std::string(" Save Scene"), MENU_ITEM_ID, "SAVE_SCENE_EDITOR").c_str(), "CTRL+S"))
					{
						editor::g_SaveScene();
					}

					if (!wasDirty)
					{
						ImGui::PopItemFlag();
						ImGui::PopStyleVar();
					}
					ImGui::EndMenu();
				}
				// TODO: Edit Icon.
				if (ImGui::BeginMenu(ImGui::IMGUI_FORMAT_ID(font::ICON_EDIT + std::string(" Edit"), MENU_ID, "CREATE_EDITOR").c_str()))
				{
					if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_MODEL + std::string(" Create Entity"), MENU_ITEM_ID, "CREATE_ENTITY_EDITOR").c_str(), "CTRL+SHIFT+N"))
					{
						editor::g_CreateEntity();
					}
					ImGui::Separator();
					if (graphics::imgui::EntityEditorSelectable* ent = dynamic_cast<graphics::imgui::EntityEditorSelectable*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get()))
					{
						if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE + std::string(" Delete Entity"), MENU_ITEM_ID, "DELETE_ENTITY_EDITOR").c_str(), "DELETE"))
						{
							ent->OnDelete();
						}
						if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_COPY + std::string(" Duplicate Entity"), MENU_ITEM_ID, "DUPLICATE_ENTITY_EDITOR").c_str(), "CTRL+D"))
						{
							// TODO: Duplicate
						}
					}
					if (graphics::imgui::FileEditorSelectable* file = dynamic_cast<graphics::imgui::FileEditorSelectable*>(core::EDITOR_ENGINE->GetEditor().GetSelectable().get()))
					{
						if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_DELETE + std::string(" Delete File"), MENU_ITEM_ID, "DELETE_ENTITY_EDITOR").c_str(), "DEL"))
						{
							file->OnDelete();
						}
						if (ImGui::MenuItem(ImGui::IMGUI_FORMAT_ID(font::ICON_FOLDER_SHOW + std::string(" Open File In Explorer"), MENU_ITEM_ID, "DUPLICATE_ENTITY_EDITOR").c_str(), "CTRL+D"))
						{
							file->OnShowInExplorer();
						}
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