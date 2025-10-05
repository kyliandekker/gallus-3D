#include "gameplay/systems/HealthSystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// HealthSystem
		//---------------------------------------------------------------------
		bool HealthSystem::Initialize()
		{
			m_UpdateTime = UpdateTime::UPDATE_TIME_POST_FRAME;

			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Health system initialized.");
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