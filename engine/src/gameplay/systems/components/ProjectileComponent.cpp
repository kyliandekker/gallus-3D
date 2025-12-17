#include "ProjectileComponent.h"

// external
#include <rapidjson/utils.h>

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/TransformSystem.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		void ProjectileComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			CollisionSystem& collisionSystem = core::ENGINE->GetECS().GetSystem<CollisionSystem>();
			std::vector<CollisionInfo> collisions = collisionSystem.GetCollisions(m_EntityID);

			HealthSystem& healthSystem = core::ENGINE->GetECS().GetSystem<HealthSystem>();

			for (auto& c : collisions)
			{
				if (c.m_CollisionType == CollisionType::COLLISION_TYPE_START)
				{
					// Get health component and decrease health.
					if (healthSystem.HasComponent(c.b))
					{
						HealthComponent& healthComponent = healthSystem.GetComponent(c.b);
						healthComponent.SetHealth(healthComponent.GetHealth() - m_fDamage);
					}

					if (auto explosionPrefab = m_ExplosionPrefab.lock())
					{
						gameplay::EntityID id = explosionPrefab->Instantiate();

						TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
						const DirectX::XMFLOAT3& pos = transformSys.GetComponent(m_EntityID).GetTransform().GetPosition();
						TransformComponent& transformComp = transformSys.GetComponent(id);
						transformComp.GetTransform().SetPosition(pos);

						// Destroy projectile on first impact.
						core::ENGINE->GetECS().GetEntity(m_EntityID)->Destroy();
					}
				}
			}

			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

			TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);
			transformComp.Translate(m_vVelocity);
		}
	}
}