#include "HealthComponent.h"

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/EntityComponentSystem.h"

namespace gallus::gameplay
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
	void HealthComponent::TakeDamage(float a_fDamage)
	{
		m_fHealth -= a_fDamage;
		if (m_fHealth < 0)
		{
			m_fHealth = 0;
		}
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
			std::shared_ptr<gameplay::Entity> ent = m_pECS->GetEntity(m_EntityID).lock();
			if (!ent)
			{
				ent->Destroy();
			}
		}
	}
}