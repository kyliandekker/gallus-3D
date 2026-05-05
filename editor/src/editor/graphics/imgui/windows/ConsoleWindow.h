#pragma once

#include "imgui_system/windows/BaseWindow.h"

// external
#include <vector>
#include <mutex>
#include <map>

// logger
#include "logger/Logger.h"

// editor
#include "editor/graphics/imgui/views/Toolbar.h"

namespace gallus::graphics::imgui
{
	class ImGuiSystem;

	inline std::mutex MESSAGE_MUTEX;

	//---------------------------------------------------------------------
	// ConsoleWindow
	//---------------------------------------------------------------------
	/// <summary>
	/// A window that displays and manages the logger messages of the project.
	/// </summary>
	class ConsoleWindow : public BaseWindow
	{
	public:
		/// <summary>
		/// Constructs a console window.
		/// </summary>
		/// <param name="a_System">The ImGui System for rendering the view.</param>
		ConsoleWindow(ImGuiSystem& a_System);

		/// <summary>
		/// Cleans up and destroys the console window.
		/// </summary>
		~ConsoleWindow();

		/// <summary>
		/// Initializes all values and behaviours associated with the console window.
		/// </summary>
		/// <returns>True if initialization is successful, otherwise false.</returns>
		bool Initialize() override;

		/// <summary>
		/// Update loop for the window. This is where all ImGui interaction should be like buttons, etc.
		/// </summary>
		void Update() override;

		/// <summary>
		/// Renders the console window.
		/// </summary>
		void Render() override;

		/// <summary>
		/// Destroys and disables the console window.
		/// </summary>
		/// <returns>True if destruction is successful, otherwise false.</returns>
		bool Destroy() override;
	private:
		// Toolbar.
		void PopulateToolbar();
		void DrawToolbar();

		/// <summary>
		/// Adds a new console message to the console window.
		/// </summary>
		/// <param name="a_Message"></param>
		void AddMessage(const logger::LoggerMessage& a_Message);

		/// <summary>
		/// Callback used to retrieve messages from the logger.
		/// </summary>
		/// <param name="a_Message"></param>
		void LoggerCallback(const logger::LoggerMessage& a_Message);

		void CountMessages();

		bool m_bNeedsRefresh = true; /// Whether the console needs to refresh the results shown in the console window.
		std::vector<logger::LoggerMessage> m_aMessages; /// List of messages retrieved from the logger.
		std::vector<size_t> m_aFilteredMessages; /// List of messages shown in the console window.
		std::vector<size_t> m_aMessageCount; /// List of messages shown in the console window.

		std::map<std::string, bool> m_aExpanded; // A list of logger message booleans.

		std::string m_sSearchBarText;

		Toolbar m_Toolbar;
	};
}