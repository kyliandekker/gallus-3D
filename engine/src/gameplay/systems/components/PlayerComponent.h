#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <map>

// gameplay
#include "gameplay/Prefab.h"

#ifdef _EDITOR
// editor
#include "editor/EditorExpose.h"
#endif

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// PlayerComponent
		//---------------------------------------------------------------------
		class PlayerComponent : public Component
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
			/// Retrieves the movement speed.
			/// </summary>
			/// <returns>Float representing the movement speed.</returns>
			float GetSpeed() const
			{
				return m_fSpeed;
			}

			/// <summary>
			/// Sets the movement speed.
			/// </summary>
			/// <param name="a_fSpeed">The movement speed.</param>
			void SetSpeed(float a_fSpeed)
			{
				m_fSpeed = a_fSpeed;
			}
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime);

			float m_fSpeed = 200;
			std::weak_ptr<gameplay::Prefab> m_pBulletPrefab = {};

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS_PARENT(PlayerComponent, Component)
				EXPOSE_FIELD(PlayerComponent, m_fSpeed, "Speed", (FieldOptions{ .type = EditorFieldWidgetType::DragFloat, .description = "The movement speed of the player. Determines how fast the player moves in units per second." }))
				EXPOSE_FIELD(PlayerComponent, m_pBulletPrefab, "Bullet Prefab", (FieldOptions{ .type = EditorFieldWidgetType::AssetPickerPtr, .assetType = resources::AssetType::Prefab, .description = "Pointer to the bullet prefab spawned when the player fires. Can be nullptr if no projectile is assigned." }))
			END_EXPOSE_FIELDS(PlayerComponent)
			BEGIN_EXPOSE_GIZMOS(PlayerComponent)
			END_EXPOSE_GIZMOS(PlayerComponent)
			END_EXPOSE_TO_EDITOR(PlayerComponent)
#endif
		};
	}
}