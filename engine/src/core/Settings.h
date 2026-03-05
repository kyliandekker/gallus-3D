#pragma once

#include "resources/ISerializableObject.h"

// external
#include <string>

namespace gallus::core
{
	//---------------------------------------------------------------------
	// Settings
	//---------------------------------------------------------------------
	/// <summary>
	/// Base class for managing application settings, providing functionality to load and save settings to persistent storage.
	/// </summary>
	class Settings : public ISerializableObject
	{
	public:
		/// <summary>
		/// Constructs settings with file name.
		/// </summary>
		/// <param name="a_sPath">The name of the settings file.</param>
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

		/// <summary>
		/// Sets the path to the file.
		/// </summary>
		/// <param name="a_sPath">The path to the settings file.</param>
		void SetPath(const std::string& a_sPath)
		{
			m_sPath = a_sPath;
		}
	private:
		std::string m_sFileName;
		std::string m_sPath;

		BEGIN_SERIALIZE(Settings)
		END_SERIALIZE(Settings)
	};
}