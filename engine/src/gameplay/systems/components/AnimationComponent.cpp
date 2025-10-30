#include "gameplay/systems/components/AnimationComponent.h"

#include <rapidjson/utils.h>

#include "core/Engine.h"

#include "resources/SrcData.h"

#define JSON_ANIMATION_COMPONENT_START_ANIMATION_VAR "startAnimation"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AnimationComponent
		//---------------------------------------------------------------------
		void AnimationComponent::InitRealtime()
		{
			if (!m_sStartAnimation.empty())
			{
				LoadAnimation(m_sStartAnimation);
			}
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
				rapidjson::Value(m_sStartAnimation.c_str(), a_Allocator),
				a_Allocator
			);
		}
#endif

		//---------------------------------------------------------------------
		void AnimationComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_sStartAnimation = a_SrcData.GetString(JSON_ANIMATION_COMPONENT_START_ANIMATION_VAR);
		}

		//---------------------------------------------------------------------
		void AnimationComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			m_AnimationTrack.Update(m_EntityID, a_fDeltaTime);
		}

		//---------------------------------------------------------------------
		void AnimationComponent::LoadAnimation(const std::string& a_sAnimName)
		{
			if (m_sAnimName == a_sAnimName)
			{
				return;
			}
			m_sAnimName = a_sAnimName;

			resources::FileResource* fileResource = nullptr;
			if (core::ENGINE->GetResourceAtlas().GetResource(m_sAnimName, fileResource))
			{
				m_AnimationTrack.LoadByPath(fileResource->GetPath());
			}
		}
	}
}