#include "HealthSystem.h"

// graphics
#include "graphics/imgui/font_icon.h"

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// HealthSystem
		//---------------------------------------------------------------------
		bool HealthSystem::Initialize()
		{
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_END);

			LOG_ICON(font::ICON_HEART, LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Health system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string HealthSystem::GetPropertyName() const
		{
			return "health";
		}

		//---------------------------------------------------------------------
		std::string HealthSystem::GetSystemName() const
		{
			std::string name = "Health";
		#ifdef _EDITOR
			name = std::string(font::ICON_HEART) + " " + name;
		#endif // _EDITOR
			return name;
		}
	}
}