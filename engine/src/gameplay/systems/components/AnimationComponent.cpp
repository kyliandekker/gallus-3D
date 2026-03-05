#include "AnimationComponent.h"

// animation
#include "animation/Animation.h"
#include "animation/AnimationKeyFrame.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// AnimationComponent
	//---------------------------------------------------------------------
	void AnimationComponent::InitRealtime()
	{
		if (m_bInitialized)
		{
			return;
		}
		if (m_pStartingAnimation.lock())
		{
			m_pAnimation = m_pStartingAnimation;
			Start();
		}
		Component::InitRealtime();
	}

	//---------------------------------------------------------------------
	void AnimationComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
	{
		if (std::shared_ptr<animation::Animation> animation = m_pAnimation.lock())
		{
			animation->Update(m_EntityID, a_fDeltaTime);
		}
	}

	//---------------------------------------------------------------------
	void AnimationComponent::LoadAnimation()
	{
		m_iNextKeyFrameIndex = 0;
		m_fAccumulatedTime = 0.0f;
	}

	//---------------------------------------------------------------------
	void AnimationComponent::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
	{
		std::shared_ptr<animation::Animation> animation = m_pAnimation.lock();
		if (!animation)
		{
			return;
		}

		if (!m_bIsPlaying)
		{
			return;
		}

		m_fAccumulatedTime += a_fDeltaTime;

		// Loop over keyframes that are due
		while (m_iNextKeyFrameIndex < animation->GetKeyFrames().size())
		{
			animation::AnimationKeyFrame* keyFrame = animation->GetKeyFrames()[m_iNextKeyFrameIndex];
			uint16_t keyFrameNumber = keyFrame->GetFrame(); // Actual frame number.
			float keyFrameTime = keyFrameNumber * animation::FRAME_TIME;

			if (m_fAccumulatedTime >= keyFrameTime)
			{
				keyFrame->Activate(m_EntityID);
				m_iNextKeyFrameIndex++;  // Only increment after successfully activating a keyframe.
			}
			else
			{
				break; // Next keyframe is not yet due.
			}
		}

		// Reset if animation is done.
		if (!animation->GetKeyFrames().empty() && m_iNextKeyFrameIndex >= animation->GetKeyFrames().size())
		{
			m_fAccumulatedTime = 0.0f;
			m_iNextKeyFrameIndex = 0;
			if (!animation->IsLooping())
			{
				m_bIsPlaying = false;
			}
		}
	}
}