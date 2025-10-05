#pragma once

#include "core/Settings.h"

#include <glm/vec2.hpp>

namespace gallus
{
	namespace graphics
	{
		namespace win32
		{
			//---------------------------------------------------------------------
			// WindowSettings
			//---------------------------------------------------------------------
			/// <summary>
			/// Manages editor settings, including window size, log visibility filters, 
			/// and tracking previously opened projects.
			/// </summary>
			class WindowSettings : public core::Settings
			{
			public:
				/// <summary>
				/// Constructs settings with file name.
				/// </summary>
				/// <param name="a_sFileName">The name the settings file should have.</param>
				WindowSettings(const std::string& a_sFileName);

				/// <summary>
				/// Sets the window size.
				/// </summary>
				/// <param name="a_Size">The new size as a glm::vec2.</param>
				void SetSize(const glm::ivec2& a_vSize);

				/// <summary>
				/// Retrieves the current editor window size.
				/// </summary>
				/// <returns>The current window size as a glm::ivec2.</returns>
				const glm::ivec2& GetSize() const;

				/// <summary>
				/// Sets the window position.
				/// </summary>
				/// <param name="a_vPosition">The new position as a glm::vec2.</param>
				void SetPosition(const glm::ivec2& a_vPosition);

				/// <summary>
				/// Retrieves the current editor window position.
				/// </summary>
				/// <returns>The current window position as a glm::ivec2.</returns>
				const glm::ivec2& GetPosition() const;
			private:
				/// <summary>
				/// Virtual method for loading specific vars.
				/// </summary>
				/// <returns>True if the settings were loaded successfully, otherwise false.</returns>
				bool LoadVars(const rapidjson::Document& a_Document);

				/// <summary>
				/// Virtual method for saving specific vars.
				/// </summary>
				/// <returns>True if the settings were saved successfully, otherwise false.</returns>
				bool SaveVars(rapidjson::Document& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const;

				glm::ivec2 m_vSize = { 400, 400 }; /// The window size.
				glm::ivec2 m_vPosition; /// The window position.
			};
		}
	}
}