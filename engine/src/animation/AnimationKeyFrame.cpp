#include "AnimationKeyFrame.h"

#include "animation/Animation.h"

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
	}
}