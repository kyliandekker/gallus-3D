#include "TransformSystem.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/systems/UpdateTime.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// TransformSystem
	//---------------------------------------------------------------------
	bool TransformSystem::Initialize()
	{
		m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_APPLY);

		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized Transform System.");
		return true;
	}

	//---------------------------------------------------------------------
	std::string TransformSystem::GetPropertyName() const
	{
		return "transform";
	}

	//---------------------------------------------------------------------
	std::string TransformSystem::GetSystemName() const
	{
		return "Transform";
	}
}