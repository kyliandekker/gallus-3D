#include "animation/AnimationKeyFrameTagComponent.h"

namespace gallus::animation
{
	//---------------------------------------------------------------------
	std::string AnimationKeyFrameTagComponent::GetName() const
	{
		return ANIMATION_KEY_FRAME_TAG_COMPONENT_NAME;
	}

	//---------------------------------------------------------------------
	std::string AnimationKeyFrameTagSystem::GetName() const
	{
		return ANIMATION_KEY_FRAME_TAG_COMPONENT_NAME;
	}
}