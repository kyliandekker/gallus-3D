#include "gameplay/systems/components/PlayerComponent.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// PlayerComponent
		//---------------------------------------------------------------------
		void PlayerComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

		}

		//---------------------------------------------------------------------
		void PlayerComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

		}
	}
}