#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <memory>
#include <DirectXMath.h>

// resources
#include "resources/AssetType.h"

namespace gallus::gameplay
{
	class Prefab;

	//---------------------------------------------------------------------
	// ProjectileComponent
	//---------------------------------------------------------------------
	class ProjectileComponent : public Component
	{
	protected:
		/// <summary>
		/// Updates the components.
		/// </summary>
		/// <param name="a_fDeltaTime">Delta time.</param>
		void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

		DirectX::XMFLOAT3 m_vVelocity = {};
		float m_fDamage;
		
		std::weak_ptr<gameplay::Prefab> m_ExplosionPrefab = {};

			BEGIN_SERIALIZE_PARENT(ProjectileComponent, Component)
			 	SERIALIZE_FIELD(m_fDamage, "Damage", "The amount of damage this projectile deals when it hits a target.",
			 		.type = FieldSerializationType::FieldSerializationType_Float)
			 	SERIALIZE_FIELD(m_ExplosionPrefab, "Explosion Prefab", "Pointer to a prefab that is spawned when the projectile explodes or impacts. Can be nullptr if no explosion effect is used.",
			 		.type = FieldSerializationType::FieldSerializationType_EngineResource,
			 		.assetType = resources::AssetType::Prefab)
			END_SERIALIZE(ProjectileComponent)
	};
}