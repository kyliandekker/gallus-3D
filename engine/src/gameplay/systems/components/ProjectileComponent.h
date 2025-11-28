#pragma once

#include "gameplay/systems/components/Component.h"

#include "editor/EditorExpose.h"

#include "gameplay/Prefab.h"

#include <DirectXMath.h>

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// ProjectileComponent
		//---------------------------------------------------------------------
		class ProjectileComponent : public Component
		{
		public:
#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;
#endif

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;

			/// <summary>
			/// Sets the velocity of the projectile.
			/// </summary>
			/// <param name="a_vVelocity">Sets the velocity of the projectile.</param>
			void SetMovementSpeed(const DirectX::XMFLOAT2& a_vVelocity)
			{
				m_vVelocity = a_vVelocity;
			}

			/// <summary>
			/// Sets the damage of the projectile.
			/// </summary>
			/// <param name="a_fDamage">The damage the projectile should do if it hits something.</param>
			void SetDamage(float a_fDamage)
			{
				m_fDamage = a_fDamage;
			}

			float GetDamage() const
			{
				return m_fDamage;
			}
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			DirectX::XMFLOAT2 m_vVelocity = {};
			float m_fDamage;
			
			gameplay::Prefab* m_ExplosionPrefab = nullptr;

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(ProjectileComponent)
				EXPOSE_FIELD(ProjectileComponent, m_fDamage, "Damage", (FieldOptions{ .type = EditorFieldWidgetType::DragFloat, .description = "The amount of damage this projectile deals when it hits a target." }))
				EXPOSE_FIELD(ProjectileComponent, m_ExplosionPrefab, "Explosion Prefab", (FieldOptions{ .type = EditorFieldWidgetType::AssetPickerPtr, .assetType = resources::AssetType::Prefab, .description = "Pointer to a prefab that is spawned when the projectile explodes or impacts. Can be nullptr if no explosion effect is used." }))
			END_EXPOSE_FIELDS(ProjectileComponent)
			BEGIN_EXPOSE_GIZMOS(ProjectileComponent)
			END_EXPOSE_GIZMOS(ProjectileComponent)
			END_EXPOSE_TO_EDITOR(ProjectileComponent)
#endif
		};
	}
}