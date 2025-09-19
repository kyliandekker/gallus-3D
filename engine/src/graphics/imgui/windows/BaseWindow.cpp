#ifndef IMGUI_DISABLE

#include "BaseWindow.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// core includes
#include "core/Tool.h"

// utils includes
#include "utils/string_extensions.h"

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// BaseWindow
			//---------------------------------------------------------------------
			BaseWindow::BaseWindow(ImGuiWindow& a_Window, ImGuiWindowFlags a_Flags, const std::string& a_sName, std::string a_sWindowID, bool a_bFullScreen) : m_Flags(a_Flags), m_sName(a_sName), m_sWindowID(a_sWindowID), m_bFullScreen(a_bFullScreen), ImGuiUIView(a_Window)
			{}

			//---------------------------------------------------------------------
			BaseWindow::~BaseWindow() = default;

			//---------------------------------------------------------------------
			bool BaseWindow::Initialize()
			{
				return true;
			}

			//---------------------------------------------------------------------
			bool BaseWindow::Destroy()
			{
				return true;
			}

			//---------------------------------------------------------------------
			bool BaseWindow::WindowBegin()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetIconFont()->FontSize, m_Window.GetIconFont()->FontSize));
				const bool b = ImGui::Begin(ImGui::IMGUI_FORMAT_ID(m_sName, WINDOW_ID, string_extensions::StringToUpper(m_sWindowID)).c_str(), &m_bEnabled, m_Flags);
				return b;
			}

			//---------------------------------------------------------------------
			void BaseWindow::WindowEnd()
			{
				ImGui::End();
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void BaseWindow::Update()
			{
				if (m_bFullScreen)
				{
					ImGuiViewport* viewport = ImGui::GetMainViewport();
					ImGui::SetNextWindowPos(ImVec2(0, 0));
					ImGui::SetNextWindowSize(viewport->Size);
				}

				bool showRender = false;
				if (!m_bRenderWindowItself)
				{
					showRender = WindowBegin();
				}

				if (showRender)
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
	}
}

#endif // IMGUI_DISABLE