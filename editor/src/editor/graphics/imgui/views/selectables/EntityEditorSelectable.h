#pragma once

#include "imgui_system/views/ImGuiUIView.h"

// external
#include <string>
#include <map>
#include <memory>

#include "gameplay/EntityID.h"

// editor
#include "editor/graphics/imgui/views/selectables/BaseEntityEditorSelectable.h"
#include "editor/graphics/imgui/EditingStateMachine.h"

namespace gallus::gameplay
{
	class Entity;
}
namespace gallus::editor
{
	class IEditorEntityCommand;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	struct ComponentEntrySelectable
	{
		bool m_bExpanded;
		std::string m_sIcon;
	};

	class EntityEditorSelectable : public BaseEntityEditorSelectable
	{
	public:
		EntityEditorSelectable(ImGuiSystem& a_System, gameplay::EntityID& a_EntityID);
		~EntityEditorSelectable();

		/// <summary>
		/// Renders the element.
		/// </summary>
		/// <param name="a_bClicked">Reference boolean that changes when clicked.</param>
		/// <param name="a_bDoubleClicked">Reference boolean that changes when double clicked.</param>
		/// <param name="a_bSelected">Whether the entity is selected.</param>
		void RenderEntity(bool& a_bClicked, bool& a_bDoubleClicked, bool a_bSelected);

		/// <summary>
		/// Renders the element.
		/// </summary>
		void Render() override
		{}

		/// <summary>
		/// Returns the entity id linked to this selectable.
		/// </summary>
		/// <returns>The entity id (Can be invalid).</returns>
		gameplay::EntityID& GetEntityID()
		{
			return m_EntityID;
		}

		/// <summary>
		/// Returns the entity id linked to this selectable.
		/// </summary>
		/// <returns>The entity id (Can be invalid).</returns>
		const gameplay::EntityID& GetEntityID() const
		{
			return m_EntityID;
		}

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

		/// <summary>
		/// Called when the selectable gets renamed.
		/// </summary>
		/// <param name="a_sDisplayName">The new name.</param>
		void OnRename(const std::string& a_sName) override;

		/// <summary>
		/// Called when the selectable gets deleted.
		/// </summary>
		void OnDelete() override;

		/// <summary>
		/// Called when the selectable gets activated.
		/// </summary>
		void OnActivate(bool a_bActive) override;

		/// <summary>
		/// Returns the name of the selectable.
		/// </summary>
		/// <returns>String containing the name.</returns>
		const std::string& GetName() const override;

		/// <summary>
		/// Sets the icon according to the highest priority component.
		/// </summary>
		void SetIcon();

		/// <summary>
		/// Called when the user double clicks on the selectable.
		/// </summary>
		void OnDoubleClicked() override;
	private:
		gameplay::EntityID m_EntityID;

		EditingStateMachine m_FieldState;
		EditingStateMachine m_GizmoState;

		std::unique_ptr<editor::IEditorEntityCommand> m_pEditorCommand;

		std::weak_ptr<gameplay::Entity> m_pEntity = {};

		std::map<std::string, ComponentEntrySelectable> m_aComponentInfo; // A list of component booleans.
	};
}