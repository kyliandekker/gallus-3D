#include "AnimationKeyFrame.h"

#include "animation/AnimationKeyFrameComponentBase.h"

namespace gallus
{
	namespace animation
	{
		//---------------------------------------------------------------------
		AnimationKeyFrame::~AnimationKeyFrame()
		{
			for (AnimationKeyFrameComponentBase* component : m_aAnimationKeyFrameComponents)
			{
				delete component;
			}
			m_aAnimationKeyFrameComponents.clear();
		}

		//---------------------------------------------------------------------
		void AnimationKeyFrame::Activate(gameplay::EntityID& a_EntityID)
		{
			for (AnimationKeyFrameComponentBase* component : m_aAnimationKeyFrameComponents)
			{
				component->Activate(a_EntityID);
			}
		}

		//---------------------------------------------------------------------
		void AnimationKeyFrame::RemoveComponent(AnimationKeyFrameComponentBase* a_pAnimationKeyFrameComponent)
		{
			size_t i = 0;
			for (AnimationKeyFrameComponentBase* keyFrame : m_aAnimationKeyFrameComponents)
			{
				if (keyFrame == a_pAnimationKeyFrameComponent)
				{
					m_aAnimationKeyFrameComponents.erase(m_aAnimationKeyFrameComponents.begin() + i);
					break;
				}
			}
		}
	}
}