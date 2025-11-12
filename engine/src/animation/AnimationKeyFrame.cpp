#include "AnimationKeyFrame.h"

#include "animation/AnimationTrack.h"

namespace gallus
{
	namespace animation
	{
		AnimationKeyFrame::~AnimationKeyFrame()
		{
			for (AnimationKeyFrameComponentBase* component : m_aAnimationKeyFrameComponents)
			{
				delete component;
			}
			m_aAnimationKeyFrameComponents.clear();
		}

		void AnimationKeyFrame::Activate(gameplay::EntityID& a_EntityID, AnimationTrack& a_AnimationTrack)
		{
			for (AnimationKeyFrameComponentBase* component : m_aAnimationKeyFrameComponents)
			{
				component->Activate(a_EntityID, a_AnimationTrack);
			}
		}

#ifdef _EDITOR
		void AnimationKeyFrame::Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			for (AnimationKeyFrameComponentBase* component : m_aAnimationKeyFrameComponents)
			{
				// Create an object for this component and let it fill it.
				rapidjson::Value componentDoc(rapidjson::kObjectType);
				component->Serialize(componentDoc, a_Allocator);

				// Create the key name as a RapidJSON string (needs allocator).
				rapidjson::Value componentName(component->GetPropertyName().c_str(), a_Allocator);

				// Add to the Components object. Use move so RapidJSON can take ownership.
				a_Value.AddMember(std::move(componentName),
					std::move(componentDoc),
					a_Allocator);
			}
		}
#endif
		void AnimationKeyFrame::Deserialize(const resources::SrcData& a_SrcData)
		{

		}
	}
}