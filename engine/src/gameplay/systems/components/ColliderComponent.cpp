#include "gameplay/systems/components/ColliderComponent.h"

#include "engineEntryPoints.h"
#include "core/Engine.h"

#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/components/TransformComponent.h"

namespace gallus::gameplay
{
	//---------------------------------------------------------------------
	// ColliderComponent
	//---------------------------------------------------------------------
	void ColliderComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
	{
		TransformSystem* transformSystem = GetEngine().GetECS()->GetSystem<TransformSystem>();
		CollisionSystem* collisionSystem = GetEngine().GetECS()->GetSystem<CollisionSystem>();

		TransformComponent* transformComponent = transformSystem->TryGetComponent(m_EntityID);
		if (!transformComponent)
		{
			return;
		}

		DirectX::XMMATRIX worldMatA = transformComponent->GetTransform().GetWorldMatrix();

		// Extract scale from matrix
		Vector3 scaleA =
		{
			worldMatA.r[0].m128_f32[0] != 0.0f ? DirectX::XMVectorGetX(DirectX::XMVector3Length(worldMatA.r[0])) : 1.0f,
			worldMatA.r[1].m128_f32[1] != 0.0f ? DirectX::XMVectorGetY(DirectX::XMVector3Length(worldMatA.r[1])) : 1.0f,
			worldMatA.r[2].m128_f32[2] != 0.0f ? DirectX::XMVectorGetZ(DirectX::XMVector3Length(worldMatA.r[2])) : 1.0f
		};

		Vector3 move = transformComponent->GetTranslation();
		Vector3 allowedMove = move;

		// Transform center using world matrix (MATCHES GIZMO)
		DirectX::XMVECTOR centerAVec = DirectX::XMVectorSet(m_ColliderSettings.m_vOffset.x, m_ColliderSettings.m_vOffset.y, m_ColliderSettings.m_vOffset.z, 1.0f);
		centerAVec = DirectX::XMVector3TransformCoord(centerAVec, worldMatA);

		Vector3 centerA =
		{
			DirectX::XMVectorGetX(centerAVec),
			DirectX::XMVectorGetY(centerAVec),
			DirectX::XMVectorGetZ(centerAVec)
		};

		// Apply scale to extents
		Vector3 halfA =
		{
			(m_ColliderSettings.m_vSize.x * 0.5f) * scaleA.x,
			(m_ColliderSettings.m_vSize.y * 0.5f) * scaleA.y,
			(m_ColliderSettings.m_vSize.z * 0.5f) * scaleA.z
		};

		auto& colliders = collisionSystem->GetComponents();

		for (auto& pair : colliders)
		{
			if (pair.first == m_EntityID)
			{
				continue;
			}

			if (m_aEntitiesToIgnore.contains(pair.first))
			{
				continue;
			}

			TransformComponent* otherTransform = transformSystem->TryGetComponent(pair.first);
			if (!otherTransform)
			{
				continue;
			}

			std::shared_ptr<ColliderComponent> other = pair.second;

			DirectX::XMMATRIX worldMatB = otherTransform->GetTransform().GetWorldMatrix();

			// Extract scale B
			Vector3 scaleB =
			{
				worldMatB.r[0].m128_f32[0] != 0.0f ? DirectX::XMVectorGetX(DirectX::XMVector3Length(worldMatB.r[0])) : 1.0f,
				worldMatB.r[1].m128_f32[1] != 0.0f ? DirectX::XMVectorGetY(DirectX::XMVector3Length(worldMatB.r[1])) : 1.0f,
				worldMatB.r[2].m128_f32[2] != 0.0f ? DirectX::XMVectorGetZ(DirectX::XMVector3Length(worldMatB.r[2])) : 1.0f
			};

			// Transform center B
			DirectX::XMVECTOR centerBVec = DirectX::XMVectorSet(other->m_ColliderSettings.m_vOffset.x, other->m_ColliderSettings.m_vOffset.y, other->m_ColliderSettings.m_vOffset.z, 1.0f);
			centerBVec = DirectX::XMVector3TransformCoord(centerBVec, worldMatB);

			Vector3 centerB =
			{
				DirectX::XMVectorGetX(centerBVec),
				DirectX::XMVectorGetY(centerBVec),
				DirectX::XMVectorGetZ(centerBVec)
			};

			Vector3 halfB =
			{
				(other->m_ColliderSettings.m_vSize.x * 0.5f) * scaleB.x,
				(other->m_ColliderSettings.m_vSize.y * 0.5f) * scaleB.y,
				(other->m_ColliderSettings.m_vSize.z * 0.5f) * scaleB.z
			};

			Vector3 minB = { centerB.x - halfB.x, centerB.y - halfB.y, centerB.z - halfB.z };
			Vector3 maxB = { centerB.x + halfB.x, centerB.y + halfB.y, centerB.z + halfB.z };

			for (int axis = 0; axis < 3; ++axis)
			{
				if (allowedMove[axis] == 0.0f)
				{
					continue;
				}

				Vector3 testCenter = centerA;
				testCenter[axis] += allowedMove[axis];

				Vector3 minA = { testCenter.x - halfA.x, testCenter.y - halfA.y, testCenter.z - halfA.z };
				Vector3 maxA = { testCenter.x + halfA.x, testCenter.y + halfA.y, testCenter.z + halfA.z };

				bool overlapX = (minA.x <= maxB.x && maxA.x >= minB.x);
				bool overlapY = (minA.y <= maxB.y && maxA.y >= minB.y);
				bool overlapZ = (minA.z <= maxB.z && maxA.z >= minB.z);

				if (!(overlapX && overlapY && overlapZ))
				{
					continue;
				}

				float penMin = maxA[axis] - minB[axis];
				float penMax = maxB[axis] - minA[axis];
				float penetration = (allowedMove[axis] > 0.0f) ? penMin : -penMax;

				allowedMove[axis] -= penetration;

				Vector3 normal = { 0.0f, 0.0f, 0.0f };
				normal[axis] = (allowedMove[axis] > 0.0f) ? 1.0f : -1.0f;

				collisionSystem->Collide(this, other, normal);
			}
		}

		transformComponent->SetTranslation(allowedMove);
	}
	
	//---------------------------------------------------------------------
	void ColliderComponent::IgnoreEntity(const gameplay::EntityID& a_EntityID)
	{
		m_aEntitiesToIgnore.insert(a_EntityID);
	}
}