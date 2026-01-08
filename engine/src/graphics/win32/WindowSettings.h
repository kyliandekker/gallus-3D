#pragma once

#include "core/Settings.h"

// utils
#include "utils/math.h"

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
				/// <param name="a_Size">The new size.</param>
				void SetSize(const IVector2& a_vSize);

				/// <summary>
				/// Retrieves the current editor window size.
				/// </summary>
				/// <returns>The current window size as a IVector2.</returns>
				const IVector2& GetSize() const;

				/// <summary>
				/// Sets the window position.
				/// </summary>
				/// <param name="a_vPosition">The new position.</param>
				void SetPosition(const IVector2& a_vPosition);

				/// <summary>
				/// Retrieves the current editor window position.
				/// </summary>
				/// <returns>The current window position.</returns>
				const IVector2& GetPosition() const;
			private:
				IVector2 m_vSize = { 400, 400 }; /// The window size.
				IVector2 m_vPosition; /// The window position.

				BEGIN_SERIALIZE_PARENT(WindowSettings, Settings)
					SERIALIZE_FIELD(m_vSize, "size", "",
						.type = FieldSerializationType::FieldSerializationType_IVector2)
					SERIALIZE_FIELD(m_vPosition, "position", "",
						.type = FieldSerializationType::FieldSerializationType_IVector2)
				END_SERIALIZE(WindowSettings)
			};
		}
	}
}