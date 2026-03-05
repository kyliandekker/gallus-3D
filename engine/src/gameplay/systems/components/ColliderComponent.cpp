#include "gameplay/systems/components/ColliderComponent.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// ColliderComponent
	//---------------------------------------------------------------------
	void ColliderComponent::UpdateRealtime(float a_fDeltatime, UpdateTime a_UpdateTime)
	{
	}
	
	//---------------------------------------------------------------------
	void ColliderComponent::IgnoreEntity(const gameplay::EntityID& a_EntityID)
	{
		m_aEntitiesToIgnore.insert(a_EntityID);
	}
}