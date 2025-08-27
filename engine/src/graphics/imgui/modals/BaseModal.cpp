#ifdef _EDITOR

#include "graphics/imgui/modals/BaseModal.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

#include "graphics/imgui/ImGuiWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			BaseModal::BaseModal(ImGuiWindow& a_Window, const std::string& a_sName, const std::string& a_sWindowID, bool a_bFullScreen) : ImGuiUIView(a_Window), m_sName(a_sName), m_sWindowID(a_sWindowID)
			{}

			bool BaseModal::ModalBegin()
			{
				return ImGui::BeginPopupModal(ImGui::IMGUI_FORMAT_ID(m_sName, POPUP_WINDOW_ID, m_sWindowID).c_str(), &m_bShow, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize);
			}

			void BaseModal::ModalEnd()
			{
				ImGui::EndPopup();
			}

			void BaseModal::Update()
			{
				if (!m_bShow)
				{
					return;
				}

				ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID(m_sName, POPUP_WINDOW_ID, m_sWindowID).c_str());

				ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
				ImVec2 size = ImGui::GetIO().DisplaySize; // Adjust as needed
				size = ImVec2(size.x * 0.5f, size.y * 0.5f);

				// Calculate the position of the top-left corner
				ImVec2 pos = ImVec2(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
				ImGui::SetNextWindowPos(pos);
				ImGui::SetNextWindowSize(size);

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetIconFont()->FontSize, m_Window.GetIconFont()->FontSize));
				if (ModalBegin())
				{
					ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
					Render();
					ModalEnd();
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			void BaseModal::Show()
			{
				m_bShow = true;
			}

			void BaseModal::Hide()
			{
				m_bShow = false;
			}
		}
	}
}

#endif // _EDITOR