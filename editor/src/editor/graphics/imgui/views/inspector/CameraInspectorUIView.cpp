#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "CameraInspectorUIView.h"

// core
#include "editor/core/EditorEngine.h"

// editor
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
				m_RotationView(a_Window),
				m_CameraProjectionModeDropdown(a_Window)
			{
				graphics::dx12::Camera& cam = core::ENGINE->GetDX12().GetCamera();
				m_CameraProjectionModeDropdown.Initialize(cam.GetCameraProjectionMode(), {
					graphics::dx12::CameraProjectionMode::Perspective,
					graphics::dx12::CameraProjectionMode::Orthographic,
					}, graphics::dx12::CameraProjectionModeToString);
			}

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
				graphics::dx12::Camera& cam = core::EDITOR_ENGINE->GetDX12().GetCamera();
				graphics::dx12::Camera& editorCam = core::EDITOR_ENGINE->GetEditor().GetEditorCamera();

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

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Camera Mode: ");
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				m_CameraProjectionModeDropdown.SetValue(cam.GetCameraProjectionMode());
				if (m_CameraProjectionModeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_FILE_INSPECTOR").c_str()))
				{
					cam.SetProjectionMode(m_CameraProjectionModeDropdown.GetValue());
					editorCam.SetProjectionMode(m_CameraProjectionModeDropdown.GetValue());
				}
				ImGui::PopStyleVar();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - m_Window.GetFramePadding().y);
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE