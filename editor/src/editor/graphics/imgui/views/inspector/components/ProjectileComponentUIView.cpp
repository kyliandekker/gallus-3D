#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ProjectileComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/imgui_toggle.h>

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
			std::string ProjectileComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void ProjectileComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "PROJECTILE_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGui::KeyValue([this]
				{
					ImGui::AlignTextToFramePadding();
					ImGui::DisplayHeader(m_Window.GetBoldFont(), "Damage: ");
				},
					[this]
				{
					float damage = m_Component.GetDamage();
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
					if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PROJECTILE_COMPONENT_DAMAGE_INSPECTOR").c_str(), &damage, 1.0f, -999999999, 99999999999))
					{
						m_Component.SetDamage(damage);
					}
				});
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