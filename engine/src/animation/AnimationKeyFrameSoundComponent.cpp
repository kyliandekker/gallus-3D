#include "animation/AnimationKeyFrameSoundComponent.h"

namespace gallus::animation
{
	//---------------------------------------------------------------------
	std::string AnimationKeyFrameSoundComponent::GetName() const
	{
		return ANIMATION_KEY_FRAME_SOUND_COMPONENT_NAME;
	}

	//---------------------------------------------------------------------
	std::string AnimationKeyFrameSoundSystem::GetName() const
	{
		return ANIMATION_KEY_FRAME_SOUND_COMPONENT_NAME;
	}
}