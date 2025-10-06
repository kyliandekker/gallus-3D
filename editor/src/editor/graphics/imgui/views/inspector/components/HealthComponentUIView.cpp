#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "HealthComponentUIView.h"

#include <imgui/imgui_helpers.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string HealthComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void HealthComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "HEALTH_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGuiWindow& window = m_Window;
				gameplay::HealthComponent& healthComp = m_Component;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Health: ");
					},
					[&healthComp]
					{
						float health = healthComp.GetHealth();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "HEALTH_COMPONENT_HEALTH_INSPECTOR").c_str(), &health, 1.0f, -999999999, 99999999999))
						{
							healthComp.SetHealth(health);
						}
					});
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Max Health: ");
					},
					[&healthComp]
					{
						float maxHealth = healthComp.GetHealth();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "HEALTH_COMPONENT_MAX_HEALTH_INSPECTOR").c_str(), &maxHealth, 1.0f, -999999999, 99999999999))
						{
							healthComp.SetMaxHealth(maxHealth);
						}
					});
				ImGui::EndInspectorKeyVal();

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE