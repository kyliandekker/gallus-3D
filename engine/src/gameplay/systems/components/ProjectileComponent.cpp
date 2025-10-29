#include "gameplay/systems/components/ProjectileComponent.h"

#include <rapidjson/utils.h>

#include "core/Engine.h"

#include "resources/SrcData.h"

#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/TransformSystem.h"

#define JSON_PROJECTILE_COMPONENT_DAMAGE_VAR "damage"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
#ifdef _EDITOR
		void ProjectileComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_PROJECTILE_COMPONENT_DAMAGE_VAR, m_fDamage, a_Allocator);
		}
#endif

		//---------------------------------------------------------------------
		void ProjectileComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_fDamage = a_SrcData.GetFloat(JSON_PROJECTILE_COMPONENT_DAMAGE_VAR);
		}

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

					// Destroy projectile on first impact.
					core::ENGINE->GetECS().GetEntity(m_EntityID)->Destroy();
				}
			}

			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

			TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);
			transformComp.Translate(m_vVelocity);
		}
	}
}