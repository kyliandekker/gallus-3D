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

			BEGIN_EXPOSED_FIELDS(ProjectileComponent)
				EXPOSE_FIELD(ProjectileComponent, m_fDamage, "Speed", FieldOptions{ .type = EditorWidgetType::DragFloat })
				EXPOSE_FIELD(ProjectileComponent, m_ExplosionPrefab, "Explosion Prefab", FieldOptions{ .type = EditorWidgetType::AssetPickerPtr, .assetType = resources::AssetType::Prefab })
			END_EXPOSED_FIELDS(ProjectileComponent)
		};
	}
}