#include "SpriteSystem.h"

// logger
#include "logger/Logger.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// SpriteSystem
	//---------------------------------------------------------------------
	bool SpriteSystem::Initialize()
	{
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized Texture System.");
		return true;
	}

	//---------------------------------------------------------------------
	std::string SpriteSystem::GetPropertyName() const
	{
		return "sprite";
	}

	//---------------------------------------------------------------------
	std::string SpriteSystem::GetSystemName() const
	{
		return "Sprite";
	}
}