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

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Gravity: ");
				ImGui::SameLine();
				bool gravity = m_Component.HasGravity();
				if (ImGui::Toggle(ImGui::IMGUI_FORMAT_ID("", CHECKBOX_ID, "MOVEMENT_GRAVITY_INSPECTOR").c_str(), &gravity))
				{
					m_Component.SetHasGravity(gravity);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}

				ImGui::PopStyleVar();

				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE