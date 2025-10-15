#include "gameplay/systems/components/HealthComponent.h"

#include <rapidjson/utils.h>

#include "resources/SrcData.h"

#define JSON_HEALTH_COMPONENT_HEALTH_VAR "health"
#define JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR "maxHealth"

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
		void HealthComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_fHealth = a_SrcData.GetFloat(JSON_HEALTH_COMPONENT_HEALTH_VAR);
			m_fMaxHealth = a_SrcData.GetFloat(JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR);
		}

		//---------------------------------------------------------------------
		void HealthComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			const Entity* entity = core::ENGINE->GetECS().GetEntity(m_EntityID);
			if (entity == nullptr)
			{
				return;
			}

			if (!entity->IsActive())
			{
				return;
			}

			if (m_fHealth <= 0)
			{
				core::ENGINE->GetECS().GetEntity(m_EntityID)->Destroy();
			}
		}
	}
}