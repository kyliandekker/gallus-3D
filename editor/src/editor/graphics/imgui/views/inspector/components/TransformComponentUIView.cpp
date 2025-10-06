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

				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "TRANSFORM_COMPONENT_TABLE_INSPECTOR"), m_Window.GetFramePadding());
				ImGuiWindow& window = m_Window;
				gameplay::TransformComponent& transformComp = m_Component;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Rotation: ");
					},
					[&transformComp]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::Text("Y");
						ImGui::SameLine();
						float rotation = transformComp.Transform().GetRotation();
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::DragFloat(ImGui::IMGUI_FORMAT_ID("", INPUT_ID, "TRANSFORM_COMPONENT_ROTATION_INSPECTOR").c_str(), &rotation, 1.0f, -999999999, 99999999999))
						{
							transformComp.Transform().SetRotation(rotation);
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				Vector2View<DirectX::XMFLOAT2>& positionView = m_PositionView;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Position: ");
					},
					[&positionView , &transformComp]
					{
						positionView.SetValue(transformComp.Transform().GetPosition());
						if (positionView.Render("TRANSFORM_COMPONENT_POSITION_INSPECTOR"))
						{
							transformComp.Transform().SetPosition(positionView.GetValue());
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				Vector2View<DirectX::XMFLOAT2>& scaleView = m_ScaleView;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Scale: ");
					},
					[&scaleView, &transformComp]
					{
						scaleView.SetValue(transformComp.Transform().GetScale());
						if (scaleView.Render("TRANSFORM_COMPONENT_SCALE_INSPECTOR"))
						{
							transformComp.Transform().SetScale(scaleView.GetValue());
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				Vector2View<DirectX::XMFLOAT2>& pivotView = m_PivotView;
				ImGui::KeyValue([&window]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(window.GetBoldFont(), "Pivot: ");
					},
					[&pivotView, &transformComp]
					{
						pivotView.SetValue(transformComp.Transform().GetPivot());
						if (pivotView.Render("TRANSFORM_COMPONENT_PIVOT_INSPECTOR"))
						{
							transformComp.Transform().SetPivot(pivotView.GetValue());
							core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
						}
					});
				ImGui::EndInspectorKeyVal();

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE