#include "Component.h"

#include "core/Engine.h"
#include "gameplay/Entity.h"

namespace gallus
{
	namespace gameplay
	{
		void Component::UpdateRealtimeInner(float a_fDeltaTime, UpdateTime a_UpdateTime)
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

			UpdateRealtime(a_fDeltaTime, a_UpdateTime);
		}
	}
}