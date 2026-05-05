#include "HealthSystem.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/systems/UpdateTime.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// HealthSystem
	//---------------------------------------------------------------------
	bool HealthSystem::Initialize()
	{
		m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_END);

		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized Health System.");
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
		return "Health";
	}
}