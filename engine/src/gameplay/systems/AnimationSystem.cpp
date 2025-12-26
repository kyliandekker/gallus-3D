#include "AnimationSystem.h"

// graphics
#include "graphics/imgui/font_icon.h"

// logger
#include "logger/Logger.h"

#include "animation/AnimationKeyFrameEventComponent.h"
#include "animation/AnimationKeyFrameSpriteComponent.h"
#include "animation/AnimationKeyFrameTagComponent.h"
#include "animation/AnimationKeyFrameSoundComponent.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AnimationSystem
		//---------------------------------------------------------------------
		bool AnimationSystem::Initialize()
		{
			m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_END);

			m_aSystems.push_back(new animation::AnimationKeyFrameTagSystem());
			m_aSystems.push_back(new animation::AnimationKeyFrameSpriteSystem());
			m_aSystems.push_back(new animation::AnimationKeyFrameSoundSystem());
			m_aSystems.push_back(new animation::AnimationKeyFrameEventSystem());

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