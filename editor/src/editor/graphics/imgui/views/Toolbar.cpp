#include "Toolbar.h"

// external
#include <imgui/imgui_internal.h>

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// Toolbar
			//---------------------------------------------------------------------
			ToolbarButton::ToolbarButton(std::function<void()> a_ButtonFunc, std::function<bool()> a_DisabledCondition) :
				m_ButtonFunc(a_ButtonFunc),
				m_DisabledCondition(a_DisabledCondition)
			{}

			//---------------------------------------------------------------------
			void ToolbarButton::Render()
			{
				bool wasDisabled = m_DisabledCondition();
				if (wasDisabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				m_ButtonFunc();
				if (wasDisabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
			}

			//---------------------------------------------------------------------
			void ToolbarBreak::Render()
			{
				ImGui::Dummy(m_vBreakSize);
			}

			//---------------------------------------------------------------------
			void Toolbar::StartToolbar()
			{
				if (m_vToolbarSize.x == 0)
				{
					m_vToolbarSize.x = ImGui::GetContentRegionAvail().x;
				}
				if (m_vToolbarSize.y == 0)
				{
					m_vToolbarSize.y = ImGui::GetContentRegionAvail().y;
				}

				m_vToolbarStartScreenPos = ImGui::GetCursorScreenPos();
				m_vToolbarStartPos = ImGui::GetCursorPos();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

				ImDrawList* draw_list = ImGui::GetWindowDrawList();

				ImVec2 header_min = m_vToolbarStartScreenPos;
				ImVec2 header_max = ImVec2(header_min.x + m_vToolbarSize.x, header_min.y + m_vToolbarSize.y);

				draw_list->AddRectFilled(header_min, header_max, ImGui::GetColorU32(ImGuiCol_Button), 0);
			}

			//---------------------------------------------------------------------
			void Toolbar::EndToolbar()
			{
				ImGui::SetCursorScreenPos({
					m_vToolbarStartScreenPos.x,
					m_vToolbarStartScreenPos.y + m_vToolbarSize.y
					});

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}

			//---------------------------------------------------------------------
			void Toolbar::Render()
			{
				for (size_t i = 0; i < m_aToolbarItems.size(); i++)
				{
					m_aToolbarItems[i]->Render();
					if (i < m_aToolbarItems.size() - 1)
					{
						ImGui::SameLine();
					}
				}
			}
		}
	}
}