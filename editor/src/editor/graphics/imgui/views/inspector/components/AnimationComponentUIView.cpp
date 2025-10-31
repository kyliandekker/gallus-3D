#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "AnimationComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/Game.h"

#include "editor/graphics/imgui/modals/FilePickerModal.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string AnimationComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void AnimationComponentUIView::RenderInner()
			{
				memset(m_sStartingAnimation, 0, sizeof(m_sStartingAnimation));
				strncpy(m_sStartingAnimation, m_Component.GetStartingAnimation().c_str(), sizeof(m_sStartingAnimation));
				m_sStartingAnimation[sizeof(m_sStartingAnimation) - 1] = '\0';

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "ANIMATION_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGuiWindow& window = m_Window;
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(window.GetFontSize() / 2, window.GetFontSize() / 2));
				ImVec2 buttonSize = ImVec2(m_Window.GetFontSize() * 2, m_Window.GetFontSize() * 2);
				FilePickerModal& filePickerModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetFilePickerModal();
				gameplay::AnimationComponent& animationComp = m_Component;
				ImGui::KeyValue([this]
				{
					ImGui::AlignTextToFramePadding();
					ImGui::DisplayHeader(m_Window.GetBoldFont(), "Starting Animation: ");
				},
					[this, &buttonSize, &filePickerModal]
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - buttonSize.x);
					ImGui::InputText(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "ANIMATION_COMPONENT_STARTING_ANIMATION_NAME_INPUT_INSPECTOR").c_str(), m_sStartingAnimation, sizeof(m_sStartingAnimation), ImGuiInputTextFlags_ReadOnly);
					ImGui::PopItemFlag();
					ImGui::SameLine();
					if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE, BUTTON_ID, "ANIMATION_COMPONENT_STARTING_ANIMATION_INSPECTOR").c_str(), buttonSize))
					{
						filePickerModal.SetData(
							[this](int success, gallus::resources::FileResource& resource)
						{
							if (success == 1)
							{
								m_Component.SetStartingAnimation(resource.GetPath().filename().generic_string().c_str());

								core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
							}
						},
							std::vector<gallus::resources::AssetType>{ gallus::resources::AssetType::Animation }
						);
						filePickerModal.Show();
					}
				});
				ImGui::PopStyleVar();
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE