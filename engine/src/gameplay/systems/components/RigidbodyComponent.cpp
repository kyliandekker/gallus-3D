#include "gameplay/systems/components/RigidbodyComponent.h"

#include <rapidjson/utils.h>

#include "core/Engine.h"

#include "resources/SrcData.h"

#include "gameplay/systems/CollisionSystem.h"

#include "gameplay/systems/TransformSystem.h"

#define JSON_RIGIDBODY_COMPONENT_USE_GRAVITY_VAR "useGravity"
#define JSON_RIGIDBODY_COMPONENT_MASS_VAR "mass"
#define JSON_RIGIDBODY_COMPONENT_LINEAR_DAMPING_VAR "linearDamping"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// RigidbodyComponent
		//---------------------------------------------------------------------
#ifdef _EDITOR
		void RigidbodyComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_RIGIDBODY_COMPONENT_USE_GRAVITY_VAR, m_bUseGravity, a_Allocator);

			a_Document.AddMember(JSON_RIGIDBODY_COMPONENT_MASS_VAR, m_fMass, a_Allocator);
			a_Document.AddMember(JSON_RIGIDBODY_COMPONENT_LINEAR_DAMPING_VAR, m_fLinearDamping, a_Allocator);
		}
#endif

		//---------------------------------------------------------------------
		void RigidbodyComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_bUseGravity = a_SrcData.GetBool(JSON_RIGIDBODY_COMPONENT_USE_GRAVITY_VAR);

			SetMass(a_SrcData.GetFloat(JSON_RIGIDBODY_COMPONENT_MASS_VAR));
			SetLinearDamping(a_SrcData.GetFloat(JSON_RIGIDBODY_COMPONENT_LINEAR_DAMPING_VAR));
		}

		constexpr float GRAVITY = 1000.0f;
		//---------------------------------------------------------------------
		void RigidbodyComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			if (!transformSys.HasComponent(m_EntityID))
			{
				return;
			}

			TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);

			if (m_bUseGravity)
			{
				m_vForce.y += GRAVITY * m_fMass;
			}

			CollisionSystem& colliderSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();

			auto collisions = colliderSys.GetCollisions(m_EntityID);
			if (!collisions.empty())
			{
				for (const auto& col : collisions)
				{
					if (col.m_vNormal.y < 0.0) // collision below
					{
						m_vVelocity.y = 0.0f;
						m_vForce.y = 0.0f;
					}
				}     
			}

			m_vAcceleration.x = m_vForce.x / m_fMass;
			m_vAcceleration.y = m_vForce.y / m_fMass;

			m_vVelocity.x += m_vAcceleration.x * a_fDeltaTime;
			m_vVelocity.y += m_vAcceleration.y * a_fDeltaTime;

			m_vVelocity.x *= m_fLinearDamping;
			m_vVelocity.y *= m_fLinearDamping;

			DirectX::XMFLOAT2 vDelta = {
				m_vVelocity.x * a_fDeltaTime,
				m_vVelocity.y * a_fDeltaTime
			};

			transformComp.Translate(vDelta);

			m_vForce = { 0.0f, 0.0f };
		}
	}
}