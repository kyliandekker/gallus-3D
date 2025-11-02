#include "animation/AnimationKeyFrameEventComponent.h"

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/CommandQueue.h"

#include "resources/SrcData.h"

#include "animation/AnimationKeyFrame.h"
#include "animation/AnimationTrack.h"

namespace gallus
{
	namespace animation
	{
		void AnimationKeyFrameEventComponent::Activate(gameplay::EntityID& a_EntityID, AnimationTrack& a_AnimationTrack)
		{
			a_AnimationTrack.ActivateEvent(a_EntityID, m_Event);
		}

#ifdef _EDITOR
		void AnimationKeyFrameEventComponent::Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator)  const
		{
			int event = (int) m_Event;
			a_Value.AddMember(
				"event",
				event,
				a_Allocator
			);
		}
#endif _EDITOR

		void AnimationKeyFrameEventComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_Event = a_SrcData.GetEnum<AnimationEvent>("event");
		}
	}
}