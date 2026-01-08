#include "WindowSettings.h"

// external
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/utils.h>

// core
#include "core/Engine.h"
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

#define DEFAULT_SIZE_X 400
#define DEFAULT_SIZE_Y 800

#define JSON_WINDOW_SIZE_VAR "windowSize"
#define JSON_WINDOW_SIZE_X_VAR "x"
#define JSON_WINDOW_SIZE_Y_VAR "y"

#define JSON_WINDOW_POSITION_VAR "windowPosition"
#define JSON_WINDOW_POSITION_X_VAR "x"
#define JSON_WINDOW_POSITION_Y_VAR "y"

namespace gallus
{
	namespace graphics
	{
		namespace win32
		{
			//---------------------------------------------------------------------
			// WindowSettings
			//---------------------------------------------------------------------
			WindowSettings::WindowSettings(const std::string& a_sFileName) : Settings(a_sFileName)
			{}

			//---------------------------------------------------------------------
			void WindowSettings::SetSize(const IVector2& a_vSize)
			{
				m_vSize = a_vSize;
			}

			//---------------------------------------------------------------------
			const IVector2& WindowSettings::GetSize() const
			{
				return m_vSize;
			}

			//---------------------------------------------------------------------
			void WindowSettings::SetPosition(const IVector2& a_vPosition)
			{
				m_vPosition = a_vPosition;
			}

			//---------------------------------------------------------------------
			const IVector2& WindowSettings::GetPosition() const
			{
				return m_vPosition;
			}
		}
	}
}