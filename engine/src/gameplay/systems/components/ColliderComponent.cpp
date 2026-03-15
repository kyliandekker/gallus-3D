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

		Vector3 move = transformComponent->GetTranslation(); // desired movement this frame
		Vector3 allowedMove = move; // what we can actually move

		Vector3 position = transformComponent->GetTransform().GetPosition();

		Vector3 centerA =
		{
			position.x + m_ColliderSettings.m_vOffset.x,
			position.y + m_ColliderSettings.m_vOffset.y,
			position.z + m_ColliderSettings.m_vOffset.z
		};

		Vector3 halfA =
		{
			m_ColliderSettings.m_vSize.x * 0.5f,
			m_ColliderSettings.m_vSize.y * 0.5f,
			m_ColliderSettings.m_vSize.z * 0.5f
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

			Vector3 otherPos = otherTransform->GetTransform().GetPosition();

			Vector3 centerB =
			{
				otherPos.x + other->m_ColliderSettings.m_vOffset.x,
				otherPos.y + other->m_ColliderSettings.m_vOffset.y,
				otherPos.z + other->m_ColliderSettings.m_vOffset.z
			};

			Vector3 halfB =
			{
				other->m_ColliderSettings.m_vSize.x * 0.5f,
				other->m_ColliderSettings.m_vSize.y * 0.5f,
				other->m_ColliderSettings.m_vSize.z * 0.5f
			};

			Vector3 minB = {
				centerB.x - halfB.x,
				centerB.y - halfB.y,
				centerB.z - halfB.z
			};
			Vector3 maxB = {
				centerB.x + halfB.x,
				centerB.y + halfB.y,
				centerB.z + halfB.z
			};

			// Check each axis separately
			for (int axis = 0; axis < 3; ++axis)
			{
				if (allowedMove[axis] == 0.0f)
				{
					continue;
				}

				// test movement along this axis only
				Vector3 testCenter = centerA;
				testCenter[axis] += allowedMove[axis];

				Vector3 minA = {
					testCenter.x - halfA.x,
					testCenter.y - halfA.y,
					testCenter.z - halfA.z
				};
				Vector3 maxA = {
					testCenter.x + halfA.x,
					testCenter.y + halfA.y,
					testCenter.z + halfA.z
				};

				bool overlapX = (minA.x <= maxB.x && maxA.x >= minB.x);
				bool overlapY = (minA.y <= maxB.y && maxA.y >= minB.y);
				bool overlapZ = (minA.z <= maxB.z && maxA.z >= minB.z);

				if (!(overlapX && overlapY && overlapZ))
				{
					continue;
				}

				// Compute minimal allowed move along this axis
				float penMin = maxA[axis] - minB[axis];
				float penMax = maxB[axis] - minA[axis];
				float penetration = (allowedMove[axis] > 0.0f) ? penMin : -penMax;

				allowedMove[axis] -= penetration; // clip movement

				// Compute collision normal for this axis
				Vector3 normal = { 0.0f, 0.0f, 0.0f };
				normal[axis] = (allowedMove[axis] > 0.0f) ? 1.0f : -1.0f;

				// Fire collision event (no push)
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

	void ColliderComponent::Init()
	{
		m_ColliderSettings.m_EntityID = m_EntityID;
		Component::Init();
	}
}