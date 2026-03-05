#include "PromptModal.h"

// external
#include <imgui/imgui.h>

// imgui
#include "imgui_system/font_icon.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// PromptModal
			//---------------------------------------------------------------------
			PromptModal::PromptModal(ImGuiSystem& a_System) : BaseModal(a_System, std::string(font::ICON_CIRCLE_INFO) + " Prompt", "Prompt")
			{}

			//---------------------------------------------------------------------
			void PromptModal::Render()
			{
				ImGui::TextWrapped(m_sQuestion.c_str());

				size_t i = 0;
				for (PromptButton& button : m_vButtons)
				{
					if (ImGui::Button(button.m_sName.c_str()))
					{
						button.m_Function();
						Hide();
					}
					if (i < m_vButtons.size() - 1)
					{
						ImGui::SameLine();
					}
					i++;
				}
			}

			//---------------------------------------------------------------------
			bool PromptModal::ModalBegin()
			{
				ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
				ImVec2 size = ImGui::GetIO().DisplaySize; // Adjust as needed
				size = ImVec2(250, 250);

				// Calculate the position of the top-left corner
				ImVec2 pos = ImVec2(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
				ImGui::SetNextWindowPos(pos);
				ImGui::SetNextWindowSize(size);

				return BaseModal::ModalBegin();
			}

			//---------------------------------------------------------------------
			void PromptModal::SetData(const std::string& a_sQuestion, const std::vector<PromptButton>& a_vButtons)
			{
				m_sQuestion = a_sQuestion;
				m_vButtons = a_vButtons;
			}
		}
	}
}