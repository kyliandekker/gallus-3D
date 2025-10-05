#include "WindowSettings.h"

// # Rapidjson
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/utils.h>

// core includes
#include "core/Engine.h"
#include "core/DataStream.h"

// logger includes
#include "logger/Logger.h"

// utils includes
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
			bool WindowSettings::LoadVars(const rapidjson::Document& a_Document)
			{
				m_vSize = glm::vec2(DEFAULT_SIZE_X, DEFAULT_SIZE_Y);

				// Window size
				if (a_Document.HasMember(JSON_WINDOW_SIZE_VAR))
				{
					glm::ivec2 temp;
					GetInt(a_Document[JSON_WINDOW_SIZE_VAR], JSON_WINDOW_SIZE_X_VAR, temp.x);
					GetInt(a_Document[JSON_WINDOW_SIZE_VAR], JSON_WINDOW_SIZE_Y_VAR, temp.y);

					if (temp.x != 0)
					{
						m_vSize.x = temp.x;
					}
					if (temp.y != 0)
					{
						m_vSize.y = temp.y;
					}
				}

				// Window position
				if (a_Document.HasMember(JSON_WINDOW_POSITION_VAR))
				{
					glm::ivec2 temp;
					GetInt(a_Document[JSON_WINDOW_POSITION_VAR], JSON_WINDOW_POSITION_X_VAR, temp.x);
					GetInt(a_Document[JSON_WINDOW_POSITION_VAR], JSON_WINDOW_POSITION_Y_VAR, temp.y);

					if (temp.x != 0)
					{
						m_vPosition.x = temp.x;
					}
					if (temp.y != 0)
					{
						m_vPosition.y = temp.y;
					}
				}

				return true;
			}

			//---------------------------------------------------------------------
			bool WindowSettings::SaveVars(rapidjson::Document& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
			{
				a_Document.AddMember(JSON_WINDOW_SIZE_VAR, rapidjson::Value().SetObject(), a_Allocator);
				a_Document[JSON_WINDOW_SIZE_VAR].AddMember(JSON_WINDOW_SIZE_X_VAR, m_vSize.x, a_Allocator);
				a_Document[JSON_WINDOW_SIZE_VAR].AddMember(JSON_WINDOW_SIZE_Y_VAR, m_vSize.y, a_Allocator);

				a_Document.AddMember(JSON_WINDOW_POSITION_VAR, rapidjson::Value().SetObject(), a_Allocator);
				a_Document[JSON_WINDOW_POSITION_VAR].AddMember(JSON_WINDOW_POSITION_X_VAR, m_vPosition.x, a_Allocator);
				a_Document[JSON_WINDOW_POSITION_VAR].AddMember(JSON_WINDOW_POSITION_Y_VAR, m_vPosition.y, a_Allocator);

				return true;
			}

			//---------------------------------------------------------------------
			void WindowSettings::SetSize(const glm::ivec2& a_vSize)
			{
				m_vSize = a_vSize;
			}

			//---------------------------------------------------------------------
			const glm::ivec2& WindowSettings::GetSize() const
			{
				return m_vSize;
			}

			//---------------------------------------------------------------------
			void WindowSettings::SetPosition(const glm::ivec2& a_vPosition)
			{
				m_vPosition = a_vPosition;
			}

			//---------------------------------------------------------------------
			const glm::ivec2& WindowSettings::GetPosition() const
			{
				return m_vPosition;
			}
		}
	}
}