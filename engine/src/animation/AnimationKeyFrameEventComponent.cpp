#include "animation/AnimationKeyFrameEventComponent.h"

// animation
#include "animation/AnimationKeyFrame.h"
#include "animation/Animation.h"

namespace gallus::animation
{
	//---------------------------------------------------------------------
	void AnimationKeyFrameEventComponent::Activate(gameplay::EntityID& a_EntityID)
	{
		m_KeyFrame.GetAnimation().ActivateEvent(a_EntityID, m_Event);
	}

	//---------------------------------------------------------------------
	std::string AnimationKeyFrameEventComponent::GetName() const
	{
		return ANIMATION_KEY_FRAME_EVENT_COMPONENT_NAME;
	}

	//---------------------------------------------------------------------
	std::string AnimationKeyFrameEventSystem::GetName() const
	{
		return ANIMATION_KEY_FRAME_EVENT_COMPONENT_NAME;
	}
}