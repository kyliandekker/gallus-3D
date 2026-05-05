#pragma once

namespace gallus::graphics::imgui
{
	class ImGuiWindowsConfig
	{
	public:
		ImGuiWindowsConfig() = default;

		/// <summary>
		/// Initializes the windows config.
		/// </summary>
		virtual void Initialize()
		{}

		/// <summary>
		/// Renders all windows, modals and docks in the config.
		/// </summary>
		virtual void Render()
		{}
	};
}