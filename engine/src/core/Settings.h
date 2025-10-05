#pragma once

#include <string>
#include <rapidjson/document.h>

namespace gallus
{
	namespace core
	{
/// Whether DX12 makes a full log of creating devices, resources, etc (spam).
#define LOG_SETTINGS 0
		//---------------------------------------------------------------------
		// Settings
		//---------------------------------------------------------------------
		/// <summary>
		/// Manages editor settings, including window size, log visibility filters, 
		/// and tracking previously opened projects.
		/// </summary>
		class Settings
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
			/// <summary>
			/// Virtual method for loading specific vars.
			/// </summary>
			/// <returns>True if the settings were loaded successfully, otherwise false.</returns>
			virtual bool LoadVars(const rapidjson::Document& a_Document) = 0;

			/// <summary>
			/// Virtual method for saving specific vars.
			/// </summary>
			/// <returns>True if the settings were saved successfully, otherwise false.</returns>
			virtual bool SaveVars(rapidjson::Document& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const = 0;

			std::string m_sFileName;
		};
	}
}