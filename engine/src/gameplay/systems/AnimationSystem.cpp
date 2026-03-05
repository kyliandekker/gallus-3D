#include "AnimationSystem.h"

// logger
#include "logger/Logger.h"

// animation
#include "animation/AnimationKeyFrameEventComponent.h"
#include "animation/AnimationKeyFrameTextureComponent.h"
#include "animation/AnimationKeyFrameTagComponent.h"
#include "animation/AnimationKeyFrameSoundComponent.h"

// gameplay
#include "gameplay/systems/UpdateTime.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// AnimationSystem
	//---------------------------------------------------------------------
	AnimationSystem::~AnimationSystem()
	{
		for (animation::AnimationKeyFrameBaseSystem* system : m_aSystems)
		{
			delete system;
		}
		m_aSystems.clear();
	}

	//---------------------------------------------------------------------
	bool AnimationSystem::Initialize()
	{
		m_aUpdateTimes.AddFlag(UpdateTime::UPDATE_TIME_FRAME_END);

		m_aSystems.push_back(new animation::AnimationKeyFrameTagSystem());
		m_aSystems.push_back(new animation::AnimationKeyFrameTextureSystem());
		m_aSystems.push_back(new animation::AnimationKeyFrameSoundSystem());
		m_aSystems.push_back(new animation::AnimationKeyFrameEventSystem());
		
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_ECS, "Successfully initialized Animation System.");
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
		return "Animation";
	}
}