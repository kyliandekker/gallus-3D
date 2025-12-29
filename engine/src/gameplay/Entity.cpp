#include "Entity.h"

#include "core/Engine.h"

namespace gallus
{
	namespace gameplay
	{
		void Entity::Destroy()
		{
			core::ENGINE->GetECS().Destroy(m_EntityID);
		}

		void Entity::SetDestroy()
		{
			m_bIsDestroyed = true;
		}
	}
}