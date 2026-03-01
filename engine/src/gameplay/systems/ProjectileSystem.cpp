#include "ProjectileSystem.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/systems/UpdateTime.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// ProjectileSystem
	//---------------------------------------------------------------------
	bool ProjectileSystem::Initialize()
	{
		m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_BEGIN);

		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized Projectile System.");
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
		return "Projectile";
	}
}