// header
#include "gameplay/systems/PlayerSystem.h"

// graphics
#include "graphics/imgui/font_icon.h"

// logger
#include "logger/Logger.h"

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