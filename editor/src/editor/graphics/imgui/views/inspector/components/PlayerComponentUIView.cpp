#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "PlayerComponentUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/ImGuizmo.h>
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// graphics includes
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay includes
#include "gameplay/systems/components/PlayerComponent.h"
#include "gameplay/systems/PlayerSystem.h"
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string PlayerComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void PlayerComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFontSize() / 2, m_Window.GetFontSize() / 2));

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Movement Speed: ");
				ImGui::SameLine();

				float speed = m_Component.GetSpeed();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "PLAYER_SPEED_INSPECTOR").c_str(), &speed, 1.0f, -999999999, 99999999999))
				{
					m_Component.SetSpeed(speed);
				}

				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE