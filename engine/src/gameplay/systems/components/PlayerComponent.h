#pragma once

#include "gameplay/systems/components/Component.h"

#include <map>

#include "gameplay/Prefab.h"

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
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;

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

			/// <summary>
			/// Retrieves the bullet prefab.
			/// </summary>
			/// <returns>Prefab containing the bullet.</returns>
			gameplay::Prefab& GetBulletPrefab()
			{
				return m_pBulletPrefab;
			}

			/// <summary>
			/// Retrieves the bullet prefab.
			/// </summary>
			/// <returns>Prefab containing the bullet.</returns>
			const gameplay::Prefab& GetBulletPrefab() const
			{
				return m_pBulletPrefab;
			}
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime);

			float m_fSpeed = 200;
			gameplay::Prefab m_pBulletPrefab;
		};
	}
}