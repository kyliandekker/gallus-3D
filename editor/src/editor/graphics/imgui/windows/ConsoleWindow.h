#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "graphics/imgui/windows/BaseWindow.h"

#include <vector>
#include <mutex>

#include "logger/Logger.h"
#include "graphics/imgui/views/DataTypes/StringTextInput.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;

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
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ConsoleWindow(ImGuiWindow& a_Window);

				/// <summary>
				/// Cleans up and destroys the console window.
				/// </summary>
				~ConsoleWindow();

				/// <summary>
				/// Renders the console window.
				/// </summary>
				void Render() override;

				/// <summary>
				/// Initializes all values and behaviours associated with the console window.
				/// </summary>
				/// <returns>True if initialization is successful, otherwise false.</returns>
				bool Initialize() override;

				/// <summary>
				/// Destroys and disables the console window.
				/// </summary>
				/// <returns>True if destruction is successful, otherwise false.</returns>
				bool Destroy() override;

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
			private:
				bool m_bNeedsRefresh = true; /// Whether the console needs to refresh the results shown in the console window.
				std::vector<logger::LoggerMessage> m_aMessages; /// List of messages retrieved from the logger.
				std::vector<size_t> m_aFilteredMessages; /// List of messages shown in the console window.

				SearchBarInput m_SearchBar; /// Search bar to filter specific messages in the console window.
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE