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

#define JSON_PROJECTILE_COMPONENT_DAMAGE_VAR "damage"
#define JSON_PROJECTILE_COMPONENT_EXPLOSION_VAR "explosion"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
#ifdef _EDITOR
		void ProjectileComponent::Serialize(resources::SrcData& a_SrcData) const
		{
			a_SrcData.SetFloat(JSON_PROJECTILE_COMPONENT_DAMAGE_VAR, m_fDamage);
			
			if (auto explosionPrefab = m_ExplosionPrefab.lock())
			{
				a_SrcData.SetString(JSON_PROJECTILE_COMPONENT_EXPLOSION_VAR, explosionPrefab->GetPath().filename().generic_string());
			}
		}
#endif

		//---------------------------------------------------------------------
		void ProjectileComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			if (a_SrcData.GetFloat(JSON_PROJECTILE_COMPONENT_DAMAGE_VAR, m_fDamage))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Projectile component did not have key %s present in its meta data.", JSON_PROJECTILE_COMPONENT_DAMAGE_VAR);
			}

			std::string explosionPrefabName = "";
			if (a_SrcData.GetString(JSON_PROJECTILE_COMPONENT_EXPLOSION_VAR, explosionPrefabName))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Projectile component did not have key %s present in its meta data.", JSON_PROJECTILE_COMPONENT_EXPLOSION_VAR);
			}

			if (!explosionPrefabName.empty())
			{
				m_ExplosionPrefab = core::ENGINE->GetResourceAtlas().LoadPrefab(explosionPrefabName);
			}
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

					if (auto explosionPrefab = m_ExplosionPrefab.lock())
					{
						gameplay::EntityID id = explosionPrefab->Instantiate();

						TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
						const DirectX::XMFLOAT3& pos = transformSys.GetComponent(m_EntityID).Transform().GetPosition();
						TransformComponent& transformComp = transformSys.GetComponent(id);
						transformComp.Transform().SetPosition(pos);

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