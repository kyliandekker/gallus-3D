// header
#include "HealthComponent.h"

// external
#include <rapidjson/utils.h>

#define JSON_HEALTH_COMPONENT_HEALTH_VAR "health"
#define JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR "maxHealth"

// core
#include "core/Engine.h"

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

		//---------------------------------------------------------------------
		void HealthComponent::UpdateRealtime(float a_fDeltaTime)
		{
			if (m_fHealth <= 0)
			{
				core::ENGINE->GetECS().GetEntity(m_EntityID)->Destroy();
			}
		}
	}
}