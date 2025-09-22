#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "TransformComponentUIView.h"

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
#include "gameplay/systems/components/TransformComponent.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			std::string TransformComponentUIView::GetName() const
			{
				return m_System.GetSystemName();
			}

			void TransformComponentUIView::RenderInner()
			{
				float fontSize = m_Window.GetFontSize();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(fontSize / 2, fontSize / 2));
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());
				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Position: ");
				ImGui::SameLine();
				ImGui::PushItemWidth(75);

				m_PositionView.SetValue(m_Component.Transform().GetPosition());
				if (m_PositionView.Render("TRANSFORM_POSITION_INSPECTOR"))
				{
					m_Component.Transform().SetPosition(m_PositionView.GetValue());
					gameplay::GAME->GetScene().SetIsDirty(true);
				}

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Rotation: ");
				ImGui::SameLine();

				float val = m_Component.Transform().GetRotation();
				if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "TRANSFORM_ROT_INSPECTOR").c_str(), &val, 1.0f, -999999999, 99999999999))
				{
					m_Component.Transform().SetRotation(val);
				}

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Scale: ");
				ImGui::SameLine();

				m_ScaleView.SetValue(m_Component.Transform().GetScale());
				if (m_ScaleView.Render("TRANSFORM_SCALE_INSPECTOR"))
				{
					m_Component.Transform().SetScale(m_ScaleView.GetValue());
					gameplay::GAME->GetScene().SetIsDirty(true);
				}

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Pivot: ");
				ImGui::SameLine();

				ImGui::PopStyleVar();
				m_PivotView.SetValue(m_Component.Transform().GetPivot());
				if (m_PivotView.Render("TRANSFORM_PIVOT_INSPECTOR", 0.01f, -0.5f, 0.5f))
				{
					m_Component.Transform().SetPivot(m_PivotView.GetValue());
					gameplay::GAME->GetScene().SetIsDirty(true);
				}

				ImGui::PopItemWidth();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE