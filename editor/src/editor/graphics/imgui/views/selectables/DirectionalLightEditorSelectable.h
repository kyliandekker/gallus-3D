#pragma once

#include "editor/graphics/imgui/views/selectables/BaseEntityEditorSelectable.h"

// external
#include <string>
#include <map>

// editor
#include "editor/graphics/imgui/EditingStateMachine.h"

namespace gallus::editor
{
	class IEditorDirectionalLightCommand;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	class DirectionalLightEditorSelectable : public BaseEntityEditorSelectable
	{
	public:
		DirectionalLightEditorSelectable(ImGuiSystem& a_System);
		~DirectionalLightEditorSelectable();

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

		std::unique_ptr<editor::IEditorDirectionalLightCommand> m_pEditorCommand;

		bool m_bExpanded = false;
		std::string m_sName = "Directional Light";
	};
}