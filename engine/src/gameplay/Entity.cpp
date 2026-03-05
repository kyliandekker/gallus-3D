#include "./Entity.h"

// core
#include "core/Engine.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"

namespace gallus::gameplay
{
	void Entity::Destroy()
	{
		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		ecs->DeleteEntity(m_EntityID);
	}
}