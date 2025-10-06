#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "MovementComponentUIView.h"

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
			std::string MovementComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void MovementComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "MOVEMENT_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGuiWindow& window = m_Window;
				gameplay::MovementComponent& movementComp = m_Component;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Gravity: ");
					},
					[&movementComp]
					{
						bool gravity = movementComp.HasGravity();
						if (ImGui::Toggle(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, "MOVEMENT_COMPONENT_GRAVITY_INSPECTOR").c_str(), &gravity))
						{
							movementComp.SetHasGravity(gravity);
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
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