#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <DirectXMath.h>

// gameplay
#include "gameplay/Prefab.h"

namespace gallus
{
	namespace gameplay
	{
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

				BEGIN_EXPOSABLE_PARENT(ProjectileComponent, Component)
				 	EXPOSE_FIELD(m_fDamage, "Damage", "The amount of damage this projectile deals when it hits a target.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Float)
				 	EXPOSE_FIELD(m_ExplosionPrefab, "Explosion Prefab", "Pointer to a prefab that is spawned when the projectile explodes or impacts. Can be nullptr if no explosion effect is used.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_EngineResource,
				 		.assetType = resources::AssetType::Prefab)
				END_EXPOSABLE(ProjectileComponent)
		};
	}
}