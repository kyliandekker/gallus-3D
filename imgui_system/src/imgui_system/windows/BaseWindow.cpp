#include "BaseWindow.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// graphics
#include "imgui_system/ImGuiSystem.h"

// utils
#include "utils/string_extensions.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	// BaseWindow
	//---------------------------------------------------------------------
	BaseWindow::BaseWindow(ImGuiSystem& a_System, ImGuiWindowFlags a_Flags, const std::string& a_sName, std::string a_sWindowID, bool a_bFullScreen) : m_Flags(a_Flags), m_sName(a_sName), m_sWindowID(a_sWindowID), m_bFullScreen(a_bFullScreen), ImGuiUIView(a_System)
	{}

	//---------------------------------------------------------------------
	BaseWindow::~BaseWindow() = default;

	//---------------------------------------------------------------------
	bool BaseWindow::Initialize()
	{
		m_bInitialized = true;
		return m_bInitialized;
	}

	//---------------------------------------------------------------------
	bool BaseWindow::Destroy()
	{
		return true;
	}

	//---------------------------------------------------------------------
	bool BaseWindow::WindowBegin()
	{
		m_bEnabled = true;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_System.GetIconFont()->FontSize, m_System.GetIconFont()->FontSize));
		if (m_bHideCloseButton)
		{
			return ImGui::Begin(ImGui::IMGUI_FORMAT_ID(m_sName, WINDOW_ID, string_extensions::StringToUpper(m_sWindowID)).c_str(), nullptr, m_Flags);;
		}
		else
		{
			return ImGui::Begin(ImGui::IMGUI_FORMAT_ID(m_sName, WINDOW_ID, string_extensions::StringToUpper(m_sWindowID)).c_str(), &m_bEnabled, m_Flags);;
		}
	}

	//---------------------------------------------------------------------
	void BaseWindow::WindowEnd()
	{
		ImDrawList* pDrawList = ImGui::GetForegroundDrawList();
		ImVec2 windowMin = ImGui::GetWindowPos();
		ImVec2 windowMax = windowMin + ImGui::GetWindowSize();

		GetOverlayQueue().Flush(pDrawList, windowMin, windowMax);

		ImGui::End();
		ImGui::PopStyleVar();
	}

	//---------------------------------------------------------------------
	void BaseWindow::Update()
	{
		if (m_bFullScreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();

			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
		}

		m_bBegin = false;
		if (!m_bRenderWindowItself)
		{
			m_bBegin = WindowBegin();
		}

		if (m_bFocusMyWindow)
		{
			ImGui::SetWindowFocus(); // focuses the current window
			m_bFocusMyWindow = false;
		}

		if (m_bBegin && m_bInitialized)
		{
			Render();
		}

		if (!m_bRenderWindowItself)
		{
			WindowEnd();
		}
	}

	//---------------------------------------------------------------------
	void BaseWindow::SetSize(ImVec2 a_vSize)
	{
		m_vSize = a_vSize;
	}

	//---------------------------------------------------------------------
	bool BaseWindow::IsFullScreen() const
	{
		return m_bFullScreen;
	}
}