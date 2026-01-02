#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <DirectXMath.h>

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
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			bool m_bUseGravity = false;

			DirectX::XMFLOAT3 m_vVelocity = { 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT3 m_vAcceleration = { 0.0f, 0.0f, 0.0f };
			DirectX::XMFLOAT3 m_vForce = { 0.0f, 0.0f, 0.0f };

			float m_fMass = 1.0f;
			float m_fLinearDamping = 0.98f;

				BEGIN_EXPOSABLE_PARENT(RigidbodyComponent, Component)
				 	EXPOSE_FIELD(m_bUseGravity, "Use Gravity", "Determines whether the object is affected by gravity. When true, the rigidbody will accelerate downward according to the physics system; when false, it will ignore gravity.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Switch)
				 	EXPOSE_FIELD(m_fMass, "Mass", "The mass of the rigidbody. Affects how forces influence the object; higher mass makes it harder to accelerate.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Float,
				 		.min = "0.0001",
				 		.max = "9999999")
				 	EXPOSE_FIELD(m_fLinearDamping, "Linear Damping", "The linear damping factor applied to the rigidbody�s velocity. Values closer to 1 reduce movement slowly, values closer to 0 slow the object more quickly over time.",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Float,
				 		.min = "0.01",
				 		.max = "1.0f")
				END_EXPOSABLE(RigidbodyComponent)
		};
	}
}