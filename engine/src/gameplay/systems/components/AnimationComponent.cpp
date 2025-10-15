#include "gameplay/systems/components/AnimationComponent.h"

#include <rapidjson/utils.h>

#include "core/Engine.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AnimationComponent
		//---------------------------------------------------------------------
		void AnimationComponent::InitRealtime()
		{
		}

		//---------------------------------------------------------------------
		void AnimationComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}
		}

		//---------------------------------------------------------------------
		void AnimationComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
		}
	}
}