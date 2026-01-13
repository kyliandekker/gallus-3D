#include "AnimationComponent.h"

// core
#include "core/Engine.h"

// animation
#include "animation/Animation.h"
#include "animation/AnimationKeyFrame.h"

// resources
#include "resources/SrcData.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
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
			if (auto animationTrack = m_pAnimation.lock())
			{
				animationTrack->Update(m_EntityID, a_fDeltaTime);
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
			auto animationTrack = m_pAnimation.lock();
			if (!animationTrack)
			{
				return;
			}

			if (!m_bIsPlaying)
			{
				return;
			}

			m_fAccumulatedTime += a_fDeltaTime;

			// Loop over keyframes that are due
			while (m_iNextKeyFrameIndex < animationTrack->GetKeyFrames().size())
			{
				animation::AnimationKeyFrame* keyFrame = animationTrack->GetKeyFrames()[m_iNextKeyFrameIndex];
				int keyFrameNumber = keyFrame->GetFrame(); // Actual frame number.
				float keyFrameTime = keyFrameNumber * FRAME_TIME;

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
			if (!animationTrack->GetKeyFrames().empty() && m_iNextKeyFrameIndex >= animationTrack->GetKeyFrames().size())
			{
				m_fAccumulatedTime = 0.0f;
				m_iNextKeyFrameIndex = 0;
				if (!animationTrack->IsLooping())
				{
					m_bIsPlaying = false;
				}
			}
		}
	}
}