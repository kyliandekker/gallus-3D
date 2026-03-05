#pragma once

namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	/// <summary>
	/// Generic UI view class used by all elements in the editor.
	/// </summary>
	class ImGuiUIView
	{
	public:
		/// <summary>
		/// Constructs a ui view for a given system.
		/// </summary>
		/// <param name="a_System">The ImGui System for rendering the view.</param>
		ImGuiUIView(ImGuiSystem& a_System) : m_System(a_System)
		{}

		virtual ~ImGuiUIView() = default;

		/// <summary>
		/// Renders the element.
		/// </summary>
		virtual void Render() = 0;

		/// <summary>
		/// Retrieves the window the element is being rendered to.
		/// </summary>
		/// <returns>Reference to the window.</returns>
		ImGuiSystem& System() const
		{
			return m_System;
		}
	protected:
		ImGuiSystem& m_System; // The window the element is being rendered to.
	};
}