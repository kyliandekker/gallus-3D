#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "TransformComponentUIView.h"

// external
#include <imgui/imgui_helpers.h>
#include <imgui/ImGuizmo.h>
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// graphics
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// gameplay
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

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Position: ");
				ImGui::Indent();

				m_PositionView.SetValue(m_Component.Transform().GetPosition());
				if (m_PositionView.Render("TRANSFORM_POSITION_INSPECTOR"))
				{
					m_Component.Transform().SetPosition(m_PositionView.GetValue());
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Rotation: ");
				ImGui::Indent();

				DirectX::XMFLOAT3 preVal = m_Component.Transform().GetRotation();
				m_RotationView.SetValue(preVal);
				int axis = m_RotationView.Render("TRANSFORM_ROTATION_INSPECTOR");
				if (axis)
				{
					DirectX::XMFLOAT3 result = {
						m_RotationView.GetValue().x - preVal.x,
						m_RotationView.GetValue().y - preVal.y,
						m_RotationView.GetValue().z - preVal.z
					};
					m_Component.Transform().AddRotation(result);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Scale: ");
				ImGui::Indent();

				m_ScaleView.SetValue(m_Component.Transform().GetScale());
				if (m_ScaleView.Render("TRANSFORM_SCALE_INSPECTOR"))
				{
					m_Component.Transform().SetScale(m_ScaleView.GetValue());
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Pivot: ");
				ImGui::Indent();

				m_PivotView.SetValue(m_Component.Transform().GetPivot());
				if (m_PivotView.Render("TRANSFORM_PIVOT_INSPECTOR", 0.01f, -0.5f, 0.5f))
				{
					m_Component.Transform().SetPivot(m_PivotView.GetValue());
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
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