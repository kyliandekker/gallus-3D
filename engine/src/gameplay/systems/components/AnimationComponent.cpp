#include "gameplay/systems/components/AnimationComponent.h"

#include <rapidjson/utils.h>

#include "core/Engine.h"

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
			if (!m_AnimationTrack)
			{
				return;
			}
			m_AnimationTrack->Update(m_EntityID, a_fDeltaTime);
		}

		//---------------------------------------------------------------------
		void AnimationComponent::LoadAnimation(const std::string& a_sAnimName)
		{
			if (m_AnimationTrack && m_AnimationTrack->GetName() == a_sAnimName)
			{
				return;
			}

			m_AnimationTrack = core::ENGINE->GetResourceAtlas().LoadAnimationTrack(a_sAnimName).get();
			m_iNextKeyFrameIndex = 0;
			m_fAccumulatedTime = 0.0f;
		}

		//---------------------------------------------------------------------
		void AnimationComponent::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			if (!m_AnimationTrack)
			{
				return;
			}

			if (!m_bIsPlaying)
			{
				return;
			}

			m_fAccumulatedTime += a_fDeltaTime;

			// Loop over keyframes that are due
			while (m_iNextKeyFrameIndex < m_AnimationTrack->GetKeyFrames().size())
			{
				animation::AnimationKeyFrame* keyFrame = m_AnimationTrack->GetKeyFrames()[m_iNextKeyFrameIndex];
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
			if (!m_AnimationTrack->GetKeyFrames().empty() && m_iNextKeyFrameIndex >= m_AnimationTrack->GetKeyFrames().size())
			{
				m_fAccumulatedTime = 0.0f;
				m_iNextKeyFrameIndex = 0;
				if (!m_AnimationTrack->IsLooping())
				{
					m_bIsPlaying = false;
				}
			}
		}
	}
}