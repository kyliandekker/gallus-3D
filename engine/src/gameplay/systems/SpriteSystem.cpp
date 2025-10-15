#include "gameplay/systems/SpriteSystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// SpriteSystem
		//---------------------------------------------------------------------
		bool SpriteSystem::Initialize()
		{
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME);

			LOG_ICON(font::ICON_IMAGE, LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Sprite system initialized.");
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
			std::string name = "Sprite";
#ifdef _EDITOR
			name = std::string(font::ICON_IMAGE) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}