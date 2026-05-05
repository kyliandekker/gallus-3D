#pragma once

#include "editor/graphics/imgui/views/selectables/BaseEntityEditorSelectable.h"

// external
#include <string>

// editor
#include "editor/graphics/imgui/EditingStateMachine.h"

namespace gallus::editor
{
	class IEditorCameraCommand;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	class CameraEditorSelectable : public BaseEntityEditorSelectable
	{
	public:
		CameraEditorSelectable(ImGuiSystem& a_System);
		~CameraEditorSelectable();

		/// <summary>
		/// Returns the name of the selectable.
		/// </summary>
		/// <returns>String containing the name.</returns>
		const std::string& GetName() const override;

		/// <summary>
		/// Renders the specific file properties.
		/// </summary>
		/// <returns>True if a field gets changed, otherwise false.</returns>
		bool RenderEditorFields() override;

		/// <summary>
		/// Renders the specific file gizmos.
		/// </summary>
		/// <returns>True if a gizmo gets changed, otherwise false.</returns>
		bool RenderGizmos(const ImRect& a_SceneViewRect) override;
	private:
		EditingStateMachine m_FieldState;
		EditingStateMachine m_GizmoState;

		std::unique_ptr<editor::IEditorCameraCommand> m_pEditorCommand;

		bool m_bExpanded = false;
		std::string m_sName = "Camera";
	};
}