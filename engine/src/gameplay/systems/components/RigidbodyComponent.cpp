#include "RigidbodyComponent.h"

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/TransformSystem.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// RigidbodyComponent
		//---------------------------------------------------------------------
		constexpr float GRAVITY = 1000.0f;
		//---------------------------------------------------------------------
		void RigidbodyComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			//TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			//if (!transformSys.HasComponent(m_EntityID))
			//{
			//	return;
			//}

			//TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);

			//if (m_bUseGravity)
			//{
			//	m_vForce.y += GRAVITY * m_fMass;
			//}

			//CollisionSystem& colliderSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();

			//auto collisions = colliderSys.GetCollisions(m_EntityID);
			//if (!collisions.empty())
			//{
			//	for (const auto& col : collisions)
			//	{
			//		if (col.m_vNormal.y < 0.0) // collision below
			//		{
			//			m_vVelocity.y = 0.0f;
			//			m_vForce.y = 0.0f;
			//		}
			//	}     
			//}

			//m_vAcceleration.x = m_vForce.x / m_fMass;
			//m_vAcceleration.y = m_vForce.y / m_fMass;
			//m_vAcceleration.z = m_vForce.z / m_fMass;

			//m_vVelocity.x += m_vAcceleration.x * a_fDeltaTime;
			//m_vVelocity.y += m_vAcceleration.y * a_fDeltaTime;
			//m_vVelocity.z += m_vAcceleration.z * a_fDeltaTime;

			//m_vVelocity.x *= m_fLinearDamping;
			//m_vVelocity.y *= m_fLinearDamping;
			//m_vVelocity.z *= m_fLinearDamping;

			//DirectX::XMFLOAT3 vDelta = {
			//	m_vVelocity.x * a_fDeltaTime,
			//	m_vVelocity.y * a_fDeltaTime,
			//	m_vVelocity.z * a_fDeltaTime
			//};

			//transformComp.Translate(vDelta);

			//m_vForce = { 0.0f, 0.0f, 0.0f };
		}
	}
}