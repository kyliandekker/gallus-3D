#include "./Entity.h"

// core
#include "core/Engine.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"

namespace gallus
{
	namespace gameplay
	{
		void Entity::Destroy()
		{
			gameplay::EntityComponentSystem* ecs = core::ENGINE->GetECS();
			if (!ecs)
			{
				return;
			}

			ecs->DeleteEntity(m_EntityID);
		}
	}
}