#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "CameraInspectorUIView.h"

// core includes
#include "editor/core/EditorEngine.h"

// editor includes
#include "editor/graphics/imgui/views/HierarchyUIView.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// CameraInspectorUIView
			//---------------------------------------------------------------------
			CameraInspectorUIView::~CameraInspectorUIView()
			{
			}

			//---------------------------------------------------------------------
			CameraInspectorUIView::CameraInspectorUIView(ImGuiWindow& a_Window, HierarchyUIView& a_HierarchyUIView) : InspectorUIView(a_Window), m_HierarchyUIView(a_HierarchyUIView),
				m_PositionView(a_Window),
				m_RotationView(a_Window)
			{ }

			//---------------------------------------------------------------------
			std::string CameraInspectorUIView::GetName() const
			{
				return m_HierarchyUIView.GetName();
			}

			//---------------------------------------------------------------------
			std::string CameraInspectorUIView::GetIcon() const
			{
				return m_HierarchyUIView.GetIcon();
			}

			//---------------------------------------------------------------------
			void CameraInspectorUIView::Render()
			{
				graphics::dx12::Camera& cam = core::ENGINE->GetDX12().GetCamera();

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetFramePadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetFramePadding());

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Position: ");
				ImGui::Indent();

				m_PositionView.SetValue(cam.Transform().GetPosition());
				if (m_PositionView.Render("TRANSFORM_POSITION_INSPECTOR"))
				{
					cam.Transform().SetPosition(m_PositionView.GetValue());
					core::EDITOR_ENGINE->GetEditor().GetScene().SetIsDirty(true);
				}
				ImGui::Unindent();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Rotation: ");
				ImGui::Indent();

				DirectX::XMFLOAT3 preVal = cam.Transform().GetRotation();
				m_RotationView.SetValue(preVal);
				int axis = m_RotationView.Render("TRANSFORM_ROTATION_INSPECTOR");
				if (axis)
				{
					DirectX::XMFLOAT3 result = {
						m_RotationView.GetValue().x - preVal.x,
						m_RotationView.GetValue().y - preVal.y,
						m_RotationView.GetValue().z - preVal.z
					};
					cam.Transform().AddRotation(result);
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