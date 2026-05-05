#pragma once

#include "imgui_system/views/ImGuiUIView.h"

// external
#include <string>
#include <map>
#include <memory>

#include "gameplay/EntityID.h"

// editor
#include "editor/graphics/imgui/views/selectables/EditorSelectable.h"

namespace gallus::gameplay
{
	class Entity;
}
namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	class BaseEntityEditorSelectable : public EditorSelectable
	{
	public:
		BaseEntityEditorSelectable(ImGuiSystem& a_System);

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

		virtual void OnActivate(bool a_bActivate) {};

		/// <summary>
		/// Returns the icon of the selectable.
		/// </summary>
		/// <returns>String containing the icon.</returns>
		const std::string& GetIcon() const override;
	protected:
		bool m_bShowActivate = false;

		bool m_bIsActive = false;

		std::string m_sIcon;
	};
}