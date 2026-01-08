#pragma once

// external
#include <string>

// editor
#include "editor/ISerializableObject.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// Settings
		//---------------------------------------------------------------------
		/// <summary>
		/// Manages editor settings, including window size, log visibility filters, 
		/// and tracking previously opened projects.
		/// </summary>
		class Settings : public ISerializableObject
		{
		public:
			/// <summary>
			/// Constructs settings with file name.
			/// </summary>
			/// <param name="a_sFileName">The name the settings file should have.</param>
			Settings(const std::string& a_sFileName);

			/// <summary>
			/// Loads settings from a persistent storage.
			/// </summary>
			/// <returns>True if the settings were loaded successfully, otherwise false.</returns>
			bool Load();

			/// <summary>
			/// Saves the current settings to persistent storage.
			/// </summary>
			/// <returns>True if the settings were saved successfully, otherwise false.</returns>
			bool Save() const;
		private:
			std::string m_sFileName;

			BEGIN_SERIALIZE(Settings)
			END_SERIALIZE(Settings)
		};
	}
}