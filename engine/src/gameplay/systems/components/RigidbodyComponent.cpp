#include "RigidbodyComponent.h"

#include "utils/math.h"

#include "engineEntryPoints.h"

#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/components/TransformComponent.h"
#include "gameplay/systems/CollisionSystem.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// RigidbodyComponent
	//---------------------------------------------------------------------
	constexpr float GRAVITY = 100.0f;
	
	//---------------------------------------------------------------------
	void RigidbodyComponent::AddForce(const DirectX::XMFLOAT2& a_vForce)
	{
		m_vForce.x += a_vForce.x;
		m_vForce.y += a_vForce.y;
	}
	
	//---------------------------------------------------------------------
	void RigidbodyComponent::SetMass(float a_fMass)
	{
		if (a_fMass <= 0.0f)
		{
			a_fMass = 0.0001f; // Prevent division by zero
		}
		m_fMass = a_fMass;
	}

	//---------------------------------------------------------------------
	void RigidbodyComponent::SetLinearDamping(float a_fLinearDamping)
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

	//---------------------------------------------------------------------
	void RigidbodyComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
	{
		if (m_bUseGravity)
		{
			m_vForce.y -= GRAVITY * m_fMass;
		}

		m_vAcceleration.x = m_vForce.x / m_fMass;
		m_vAcceleration.y = m_vForce.y / m_fMass;
		m_vAcceleration.z = m_vForce.z / m_fMass;

		CollisionSystem* colliderSys = GetEngine().GetECS()->GetSystem<CollisionSystem>();
		if (colliderSys)
		{
			auto collisions = colliderSys->GetCollisions(m_EntityID);
			if (!collisions.empty())
			{
				for (const auto& col : collisions)
				{
					if (col.m_vNormal.y < 0) // collision below
					{
						m_vVelocity.y = 0.0f;
						m_vForce.y = 0.0f;
					}
				}
			}
		}

		m_vVelocity.x += m_vAcceleration.x * a_fDeltaTime;
		m_vVelocity.y += m_vAcceleration.y * a_fDeltaTime;
		m_vVelocity.z += m_vAcceleration.z * a_fDeltaTime;

		m_vVelocity.x *= m_fLinearDamping;
		m_vVelocity.y *= m_fLinearDamping;
		m_vVelocity.z *= m_fLinearDamping;

		Vector3 translation;
		translation.x += m_vVelocity.x * a_fDeltaTime;
		translation.y += m_vVelocity.y * a_fDeltaTime;
		translation.z += m_vVelocity.z * a_fDeltaTime;

		if (TransformComponent* transformComponent = GetEngine().GetECS()->GetSystem<TransformSystem>()->TryGetComponent(m_EntityID))
		{
			transformComponent->Translate(translation);
		}

		m_vForce.x = 0.0f;
		m_vForce.y = 0.0f;
		m_vForce.z = 0.0f;
	}
}