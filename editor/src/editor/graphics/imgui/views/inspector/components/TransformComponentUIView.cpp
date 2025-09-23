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

			void TransformComponentUIView::RenderComponentGizmos(const ImVec2& a_vScenePos, const ImVec2& a_vSize, const ImVec2& a_vPanOffset, float a_fZoom)
			{
				DrawGizmos(a_vScenePos, a_vSize, a_vPanOffset, a_fZoom);
				DrawTransformGizmo(m_Component.Transform(), a_vScenePos, a_vSize, a_vPanOffset, a_fZoom);
			}

			void TransformComponentUIView::RenderInner()
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Rotation: ");
				ImGui::SameLine();

				float rotation = m_Component.Transform().GetRotation();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "TRANSFORM_ROT_INSPECTOR").c_str(), &rotation, 1.0f, -999999999, 99999999999))
				{
					m_Component.Transform().SetRotation(rotation);
				}

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Position: ");
				ImGui::Indent();

				m_PositionView.SetValue(m_Component.Transform().GetPosition());
				if (m_PositionView.Render("TRANSFORM_POSITION_INSPECTOR"))
				{
					m_Component.Transform().SetPosition(m_PositionView.GetValue());
					gameplay::GAME.GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Scale: ");
				ImGui::Indent();

				m_ScaleView.SetValue(m_Component.Transform().GetScale());
				if (m_ScaleView.Render("TRANSFORM_SCALE_INSPECTOR"))
				{
					m_Component.Transform().SetScale(m_ScaleView.GetValue());
					gameplay::GAME.GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Pivot: ");
				ImGui::Indent();

				m_PivotView.SetValue(m_Component.Transform().GetPivot());
				if (m_PivotView.Render("TRANSFORM_PIVOT_INSPECTOR", 0.01f, -0.5f, 0.5f))
				{
					m_Component.Transform().SetPivot(m_PivotView.GetValue());
					gameplay::GAME.GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE