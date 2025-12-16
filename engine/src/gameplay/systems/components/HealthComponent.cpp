#include "HealthComponent.h"

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

#include "logger/Logger.h"

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
			if (m_bInitialized)
			{
				return;
			}
			m_fHealth = m_fMaxHealth;

			Component::InitRealtime();
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		void HealthComponent::Serialize(resources::SrcData& a_SrcData) const
		{
			a_SrcData.SetFloat(JSON_HEALTH_COMPONENT_HEALTH_VAR, m_fHealth);
			a_SrcData.SetFloat(JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR, m_fMaxHealth);
		}
#endif

		//---------------------------------------------------------------------
		void HealthComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			if (!a_SrcData.GetFloat(JSON_HEALTH_COMPONENT_HEALTH_VAR, m_fHealth))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Health component did not have key %s present in its meta data.", JSON_HEALTH_COMPONENT_HEALTH_VAR);
			}
			if (!a_SrcData.GetFloat(JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR, m_fMaxHealth))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Health component did not have key %s present in its meta data.", JSON_HEALTH_COMPONENT_MAX_HEALTH_VAR);
			}
		}

		//---------------------------------------------------------------------
		void HealthComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			if (m_fHealth <= 0)
			{
				core::ENGINE->GetECS().GetEntity(m_EntityID)->Destroy();
			}
		}
	}
}