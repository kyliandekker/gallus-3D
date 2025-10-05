#include "gameplay/systems/PlayerSystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// PlayerSystem
		//---------------------------------------------------------------------
		bool PlayerSystem::Initialize()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Player system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string PlayerSystem::GetPropertyName() const
		{
			return "player";
		}

		//---------------------------------------------------------------------
		std::string PlayerSystem::GetSystemName() const
		{
			std::string name = "Player";
		#ifdef _EDITOR
			name = std::string(font::ICON_SETTINGS) + " " + name;
		#endif // _EDITOR
			return name;
		}
	}
}