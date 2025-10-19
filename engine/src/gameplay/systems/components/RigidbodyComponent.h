#pragma once

#include <DirectXMath.h>

#include "gameplay/systems/components/Component.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// RigidbodyComponent
		//---------------------------------------------------------------------
		class RigidbodyComponent : public Component
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
			/// Retrieves whether the rigidbody should use gravity or not.
			/// </summary>
			/// <returns>True if gravity is enabled, false otherwise.</returns>
			bool UseGravity() const
			{
				return m_bUseGravity;
			}

			/// <summary>
			/// Sets whether the rigidbody should use gravity or not.
			/// </summary>
			/// <param name="a_bUseGravity">True to enable gravity, false otherwise.</param>
			void SetUseGravity(bool a_bUseGravity)
			{
				m_bUseGravity = a_bUseGravity;
			}

			/// <summary>
			/// Adds force to the rigidbody.
			/// </summary>
			/// <param name="a_vForce">Velocity to add.</param>
			void AddForce(const DirectX::XMFLOAT2& a_vForce)
			{
				m_vForce.x += a_vForce.x;
				m_vForce.y += a_vForce.y;
			}

			void SetMass(float a_fMass)
			{
				if (a_fMass <= 0.0f)
				{
					a_fMass = 0.0001f; // Prevent division by zero
				}
				m_fMass = a_fMass;
			}

			float GetMass()
			{
				return m_fMass;
			}

			void SetLinearDamping(float a_fLinearDamping)
			{
				if (a_fLinearDamping < 0.0f)
				{
					a_fLinearDamping = 0.0f;
				}
				else if (a_fLinearDamping > 1.0f)
				{
					a_fLinearDamping = 1.0f;
				}
				m_fLinearDamping = a_fLinearDamping;
			}

			float GetLinearDamping()
			{
				return m_fLinearDamping;
			}
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			bool m_bUseGravity = false;

			DirectX::XMFLOAT2 m_vVelocity = { 0.0f, 0.0f };
			DirectX::XMFLOAT2 m_vAcceleration = { 0.0f, 0.0f };
			DirectX::XMFLOAT2 m_vForce = { 0.0f, 0.0f };

			float m_fMass = 1.0f;
			float m_fLinearDamping = 0.98f;
		};
	}
}