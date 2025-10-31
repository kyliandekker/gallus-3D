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
#include <editor/graphics/imgui/EditorWindowsConfig.h>

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

				FilePickerModal& filePickerModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "PLAYER_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding() / 2);
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Movement Speed: ");
					},
					[this]
					{
						float speed = m_Component.GetSpeed();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PLAYER_COMPONENT_SPEED_INSPECTOR").c_str(), &speed, 1.0f, -999999999, 99999999999))
						{
							m_Component.SetSpeed(speed);
						}
					});
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));

				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Bullet Prefab: ");
					},
					[this, &filePickerModal]
					{
						ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
						ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);
						ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
						ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PLAYER_COMPONENT_BULLET_PREFAB_NAME_INPUT_INSPECTOR").c_str(), m_sPrefabName, sizeof(m_sPrefabName), ImGuiInputTextFlags_ReadOnly);
						ImGui::PopItemFlag();
						ImGui::SameLine();
						if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "PLAYER_COMPONENT_BULLET_PREFAB_INSPECTOR").c_str(), buttonSize))
						{
							filePickerModal.SetData(
								[this](int success, gallus::resources::FileResource& resource)
								{
									if (success == 1)
									{
										m_Component.GetBulletPrefab().SetPath(resource.GetPath().filename().generic_string());
										m_Component.GetBulletPrefab().Load();
									}
								},
								std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Prefab }
							);
							filePickerModal.Show();
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