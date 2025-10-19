#include "gameplay/systems/components/ProjectileComponent.h"

#include <rapidjson/utils.h>

#include "core/Engine.h"
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		void ProjectileComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}
		}

		//---------------------------------------------------------------------
		void ProjectileComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
		}

		//---------------------------------------------------------------------
		void ProjectileComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			CollisionSystem& collisionSystem = core::ENGINE->GetECS().GetSystem<CollisionSystem>();
			auto collisions = collisionSystem.GetCollisions(m_EntityID);

			HealthSystem& healthSystem = core::ENGINE->GetECS().GetSystem<HealthSystem>();

			for (auto& c : collisions)
			{
				if (c.m_CollisionType == CollisionType::COLLISION_TYPE_START)
				{
					// Get health component and decrease health.
					if (healthSystem.HasComponent(c.b))
					{
						HealthComponent& healthComponent = healthSystem.GetComponent(c.b);
						healthComponent.SetHealth(healthComponent.GetHealth() - 100);
					}

					// Destroy projectile on first impact.
					core::ENGINE->GetECS().GetEntity(m_EntityID)->Destroy();
				}
			}

			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

			TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);
			transformComp.Translate(m_fVelocity);
		}
	}
}