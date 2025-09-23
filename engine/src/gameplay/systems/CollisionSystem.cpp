#include "gameplay/systems/CollisionSystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// CollisionSystem
		//---------------------------------------------------------------------
		bool CollisionSystem::Initialize()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Collision system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string CollisionSystem::GetPropertyName() const
		{
			return "collision";
		}

		//---------------------------------------------------------------------
		std::string CollisionSystem::GetSystemName() const
		{
			std::string name = "Collision";
		#ifdef _EDITOR
			name = std::string(font::ICON_BOUNDS) + " " + name;
		#endif // _EDITOR
			return name;
		}
	}
}