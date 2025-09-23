#include "gameplay/systems/components/HealthComponent.h"

#include <rapidjson/utils.h>

#define JSON_HEALTH_COMPONENT_HEALTH_VAR "health"
#define JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR "maxHealth"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// HealthComponent
		//---------------------------------------------------------------------
		void HealthComponent::InitRealtime()
		{
			m_fHealth = m_fMaxHealth;
		}

		//---------------------------------------------------------------------
		void HealthComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_HEALTH_COMPONENT_HEALTH_VAR, m_fHealth, a_Allocator);
			a_Document.AddMember(JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR, m_fMaxHealth, a_Allocator);
		}

		//---------------------------------------------------------------------
		void HealthComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			rapidjson::GetFloat(a_Document, JSON_HEALTH_COMPONENT_HEALTH_VAR, m_fHealth);
			rapidjson::GetFloat(a_Document, JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR, m_fMaxHealth);
		}
	}
}