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
			if (!m_pECS)
			{
				return;
			}

			if (m_fHealth <= 0)
			{
				auto ent = m_pECS->GetEntity(m_EntityID).lock();
				if (!ent)
				{
					ent->Destroy();
				}
			}
		}
	}
}