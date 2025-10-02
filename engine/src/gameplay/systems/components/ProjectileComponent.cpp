// header
#include "ProjectileComponent.h"

// external
#include <rapidjson/utils.h>

// core
#include "core/Engine.h"

// gameplay
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/MovementSystem.h"

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
		void ProjectileComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}
		}

		//---------------------------------------------------------------------
		void ProjectileComponent::UpdateRealtime(float a_fDeltaTime)
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

			MovementSystem& movementSys = core::ENGINE->GetECS().GetSystem<MovementSystem>();

			MovementComponent& movementComp = movementSys.GetComponent(m_EntityID);
			movementComp.Translate(m_fVelocity);
		}
	}
}