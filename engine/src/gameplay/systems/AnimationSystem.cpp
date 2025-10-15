#include "gameplay/systems/AnimationSystem.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/font_icon.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AnimationSystem
		//---------------------------------------------------------------------
		bool AnimationSystem::Initialize()
		{
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_POST_FRAME);

			LOG_ICON(font::ICON_ANIMATION, LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Animation system initialized.");
			return true;
		}

		//---------------------------------------------------------------------
		std::string AnimationSystem::GetPropertyName() const
		{
			return "animation";
		}

		//---------------------------------------------------------------------
		std::string AnimationSystem::GetSystemName() const
		{
			std::string name = "Animation";
#ifdef _EDITOR
			name = std::string(font::ICON_ANIMATION) + " " + name;
#endif // _EDITOR
			return name;
		}
	}
}