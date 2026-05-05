#include "RigidbodySystem.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/systems/UpdateTime.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// RigidbodySystem
	//---------------------------------------------------------------------
	bool RigidbodySystem::Initialize()
	{
		m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME);

		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized Rigidbody System.");
		return true;
	}

	//---------------------------------------------------------------------
	std::string RigidbodySystem::GetPropertyName() const
	{
		return "rigidbody";
	}

	//---------------------------------------------------------------------
	std::string RigidbodySystem::GetSystemName() const
	{
		return "Rigidbody";
	}
}