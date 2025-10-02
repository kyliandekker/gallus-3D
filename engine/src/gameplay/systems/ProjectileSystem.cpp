// header
#include "ProjectileSystem.h"

// graphics
#include "graphics/imgui/font_icon.h"

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// ProjectileSystem
		//---------------------------------------------------------------------
		bool ProjectileSystem::Initialize()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Projectile system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string ProjectileSystem::GetPropertyName() const
		{
			return "projectile";
		}

		//---------------------------------------------------------------------
		std::string ProjectileSystem::GetSystemName() const
		{
			std::string name = "Projectile";
#ifdef _EDITOR
			name = std::string(font::ICON_HEART) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}