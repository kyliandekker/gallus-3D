#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "PlayerComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/ImGuizmo.h>
#include <imgui/imgui_internal.h>
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// editor includes
#include "editor/core/EditorEngine.h"
#include "resources/AssetType.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"

// gameplay includes
#include "gameplay/systems/components/PlayerComponent.h"
#include "gameplay/systems/PlayerSystem.h"
#include "gameplay/Game.h"

// resources
#include "resources/FileResource.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string PlayerComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void PlayerComponentUIView::RenderInner()
			{
				memset(m_sPrefabName, 0, sizeof(m_sPrefabName));
				std::string prefabName = m_Component.GetBulletPrefab().GetPath().stem().generic_string();
				strncpy(m_sPrefabName, prefabName.c_str(), sizeof(m_sPrefabName));
				m_sPrefabName[sizeof(m_sPrefabName) - 1] = '\0';

				FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal((int) EDITOR_MODAL::EDITOR_MODAL_FILE_PICKER));

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "PLAYER_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding() / 2);
				ImGuiWindow& window = m_Window;
				gameplay::PlayerComponent& playerComp = m_Component;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Movement Speed: ");
					},
					[&playerComp]
					{
						float speed = playerComp.GetSpeed();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PLAYER_COMPONENT_SPEED_INSPECTOR").c_str(), &speed, 1.0f, -999999999, 99999999999))
						{
							playerComp.SetSpeed(speed);
						}
					});
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(window.GetFontSize() / 2, window.GetFontSize() / 2));

				char* prefabNameStr = m_sPrefabName;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Bullet Prefab: ");
					},
					[&playerComp, modal, &window, prefabNameStr]
					{
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
						ImVec2 buttonSize = ImVec2(window.GetFontSize() * 2, window.GetFontSize() * 2);
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PLAYER_COMPONENT_BULLET_PREFAB_NAME_INPUT_INSPECTOR").c_str(), prefabNameStr, sizeof(prefabNameStr), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "PLAYER_COMPONENT_BULLET_PREFAB_INSPECTOR").c_str(), buttonSize))
						{
							if (modal)
							{
								modal->SetData(
									[&playerComp](int success, gallus::resources::FileResource& resource)
									{
										if (success == 1)
										{
											playerComp.GetBulletPrefab().SetPath(resource.GetPath().filename().generic_string());
											playerComp.GetBulletPrefab().Load();
										}
									},
									std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Prefab }
								);
								modal->Show();
							}
						}
						ImGui::PopStyleVar();
					});
				ImGui::PopStyleVar();
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE