#include "BaseModal.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <imgui/imgui_internal.h>

// graphics
#include "imgui_system/ImGuiSystem.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// BaseModal
	//---------------------------------------------------------------------
	BaseModal::BaseModal(ImGuiSystem& a_System, const std::string& a_sName, const std::string& a_sWindowID, bool a_bFullScreen) : ImGuiUIView(a_System), m_sName(a_sName), m_sWindowID(a_sWindowID)
	{}

	//---------------------------------------------------------------------
	bool BaseModal::Initialize()
	{
		return true;
	}

	//---------------------------------------------------------------------
	bool BaseModal::ModalBegin()
	{
		return ImGui::BeginPopupModal(ImGui::IMGUI_FORMAT_ID(m_sName, POPUP_WINDOW_ID, m_sWindowID).c_str(), &m_bShow, ImGuiWindowFlags_Modal | ImGuiWindowFlags_AlwaysAutoResize);
	}

	//---------------------------------------------------------------------
	void BaseModal::ModalEnd()
	{
		ImGui::EndPopup();
	}

	//---------------------------------------------------------------------
	void BaseModal::Update()
	{
		if (!m_bShow)
		{
			return;
		}

		if (m_bCallShow)
		{
			ImGui::OpenPopup(ImGui::IMGUI_FORMAT_ID(m_sName, POPUP_WINDOW_ID, m_sWindowID).c_str());
			m_bCallShow = false;
		}

		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImVec2 size = ImGui::GetIO().DisplaySize; // Adjust as needed
		size = ImVec2(size.x * 0.75f, size.y * 0.75f);

		// Calculate the position of the top-left corner
		ImVec2 pos = ImVec2(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
		ImGui::SetNextWindowPos(pos);
		ImGui::SetNextWindowSize(size);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_System.GetFramePadding());
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_System.GetIconFont()->FontSize, m_System.GetIconFont()->FontSize));
		if (ModalBegin())
		{
			ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());
			Render();
			ModalEnd();
		}
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}

	//---------------------------------------------------------------------
	void BaseModal::Show()
	{
		m_bShow = true;
		m_bCallShow = true;
	}

	//---------------------------------------------------------------------
	void BaseModal::Hide()
	{
		m_bShow = false;
		m_bCallShow = false;
	}
}