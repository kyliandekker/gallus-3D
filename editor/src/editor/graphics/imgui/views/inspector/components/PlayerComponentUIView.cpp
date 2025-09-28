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
#include "editor/FileResource.h"
#include "editor/graphics/imgui/modals/FilePickerModal.h"

// gameplay includes
#include "gameplay/systems/components/PlayerComponent.h"
#include "gameplay/systems/PlayerSystem.h"
#include "gameplay/Game.h"

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
				std::string prefabName = m_Component.GetPrefab().GetPath().stem().generic_string();
				strncpy(m_sPrefabName, prefabName.c_str(), sizeof(m_sPrefabName));
				m_sPrefabName[sizeof(m_sPrefabName) - 1] = '\0';

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Movement Speed: ");
				ImGui::SameLine();

				float speed = m_Component.GetSpeed();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PLAYER_SPEED_INSPECTOR").c_str(), &speed, 1.0f, -999999999, 99999999999))
				{
					m_Component.SetSpeed(speed);
				}

				ImGui::PopStyleVar();

				FilePickerModal* modal = dynamic_cast<FilePickerModal*>(m_Window.GetModal((int) EDITOR_MODAL::EDITOR_MODAL_FILE_PICKER));

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Prefab Name: ");
				ImGui::SameLine();
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
				ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PLAYER_PREFAB_NAME_INPUT").c_str(), m_sPrefabName, sizeof(m_sPrefabName), ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemFlag();
				ImGui::SameLine();
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "PLAYER_PREFAB").c_str(), buttonSize))
				{
					if (modal)
					{
						modal->SetData(
							[this](int success, gallus::editor::FileResource& resource)
							{
								if (success == 1)
								{
									m_Component.GetPrefab().SetPath(resource.GetPath().filename().generic_string());
									m_Component.GetPrefab().Load();
								}
							},
							std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Prefab }
						);
						modal->Show();
					}
				}

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + m_Window.GetFramePadding().y);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE