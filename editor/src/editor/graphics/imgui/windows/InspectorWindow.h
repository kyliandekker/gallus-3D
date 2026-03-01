#pragma once

#include "imgui_system/windows/BaseWindow.h"

// external
#include <vector>
#include <string>
#include <memory>

// graphics
#include "editor/graphics/imgui/views/StringTextInput.h"

// editor
#include "editor/graphics/imgui/views/selectables/FileEditorSelectable.h"
#include "editor/graphics/imgui/views/Toolbar.h"

namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	class InspectorView;

	/// <summary>
	/// A window that displays the current editor selectable.
	/// </summary>
	class InspectorWindow : public BaseWindow
	{
	public:
		/// <summary>
		/// Constructs an inspector window.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		InspectorWindow(ImGuiSystem& a_System);

		/// <summary>
		/// Initializes all behaviours and values for the window.
		/// </summary>
		/// <returns>True if initialization is successful, otherwise false.</returns>
		bool Initialize() override;

		/// <summary>
		/// Destroys and disables the inspector window.
		/// </summary>
		/// <returns>True if destruction is successful, otherwise false.</returns>
		bool Destroy() override;

		/// <summary>
		/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
		/// </summary>
		void Update() override;

		/// <summary>
		/// Renders the inspector window.
		/// </summary>
		void Render() override;
	private:
		// Toolbar.
		void PopulateToolbar();
		void DrawToolbar();

		StringTextInput m_NameInput; /// Inspector input for editing the entity's name.

		Toolbar m_Toolbar;

		bool m_bDeleteSelectable = false;
	};
}