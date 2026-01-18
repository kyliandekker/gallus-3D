#include "./Entity.h"

#include "core/Engine.h"

namespace gallus
{
	namespace gameplay
	{
		void Entity::Destroy()
		{
			core::ENGINE->GetECS().DeleteEntity(m_EntityID);
		}
	}
}