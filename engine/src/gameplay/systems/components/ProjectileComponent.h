#pragma once

#include "gameplay/systems/components/Component.h"

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
			/// Sets the velocity of the projectile.
			/// </summary>
			/// <param name="a_fVelocity">Sets the velocity of the projectile.</param>
			void SetMovementSpeed(const DirectX::XMFLOAT2& a_fVelocity)
			{
				m_fVelocity = a_fVelocity;
			}
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			DirectX::XMFLOAT2 m_fVelocity = {};
		};
	}
}