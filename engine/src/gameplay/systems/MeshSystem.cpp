#include "MeshSystem.h"

// logger
#include "logger/Logger.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// MeshSystem
	//---------------------------------------------------------------------
	bool MeshSystem::Initialize()
	{
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized Mesh System.");
		return true;
	}

	//---------------------------------------------------------------------
	std::string MeshSystem::GetPropertyName() const
	{
		return "mesh";
	}

	//---------------------------------------------------------------------
	std::string MeshSystem::GetSystemName() const
	{
		return "Mesh";
	}
}