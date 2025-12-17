#include "HealthComponent.h"

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

#include "logger/Logger.h"

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
		void HealthComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			if (m_fHealth <= 0)
			{
				core::ENGINE->GetECS().GetEntity(m_EntityID)->Destroy();
			}
		}
	}
}