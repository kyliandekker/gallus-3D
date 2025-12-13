#include "AnimationComponent.h"

// external
#include <rapidjson/utils.h>

// core
#include "core/Engine.h"

// animation
#include "animation/AnimationTrack.h"

// resources
#include "resources/SrcData.h"

#define JSON_ANIMATION_COMPONENT_START_ANIMATION_VAR "startingAnimation"

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
			if (!m_sStartingAnimation.empty())
			{
				LoadAnimation(m_sStartingAnimation);
				Start();
			}
			Component::InitRealtime();
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		void AnimationComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(
				JSON_ANIMATION_COMPONENT_START_ANIMATION_VAR,
				rapidjson::Value(m_sStartingAnimation.c_str(), a_Allocator),
				a_Allocator
			);
		}
#endif

		//---------------------------------------------------------------------
		void AnimationComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_sStartingAnimation = a_SrcData.GetString(JSON_ANIMATION_COMPONENT_START_ANIMATION_VAR);
		}

		//---------------------------------------------------------------------
		void AnimationComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			if (auto animationTrack = m_AnimationTrack.lock())
			{
				animationTrack->Update(m_EntityID, a_fDeltaTime);
			}
		}

		//---------------------------------------------------------------------
		void AnimationComponent::LoadAnimation(const std::string& a_sAnimName)
		{
			if (auto animationTrack = m_AnimationTrack.lock())
			{
				if (animationTrack->GetName() == a_sAnimName)
				{
					return;
				}
			}

			m_AnimationTrack = core::ENGINE->GetResourceAtlas().LoadAnimationTrack(a_sAnimName);
			m_iNextKeyFrameIndex = 0;
			m_fAccumulatedTime = 0.0f;
		}

		//---------------------------------------------------------------------
		void AnimationComponent::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			auto animationTrack = m_AnimationTrack.lock();
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
				int keyFrameNumber = keyFrame->GetFrame(); // actual frame number
				float keyFrameTime = keyFrameNumber * FRAME_TIME;

				if (m_fAccumulatedTime >= keyFrameTime)
				{
					keyFrame->Activate(m_EntityID);
					m_iNextKeyFrameIndex++;  // only increment after successfully activating a keyframe
				}
				else
				{
					break; // next keyframe is not yet due
				}
			}

			// Reset if animation is done
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