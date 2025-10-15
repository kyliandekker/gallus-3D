#include "gameplay/systems/components/ColliderComponent.h"

#include <rapidjson/utils.h>

#include "resources/SrcData.h"

#include "core/Engine.h"

#include "gameplay/Entity.h"
#include "gameplay/systems/MovementSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/CollisionSystem.h"

#define JSON_COLLIDER_COMPONENT_OFFSET_VAR "offset"
#define JSON_COLLIDER_COMPONENT_SIZE_VAR "size"
#define JSON_COLLIDER_COMPONENT_X_VAR "x"
#define JSON_COLLIDER_COMPONENT_Y_VAR "y"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// ColliderComponent
		//---------------------------------------------------------------------
		void ColliderComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_COLLIDER_COMPONENT_OFFSET_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_COLLIDER_COMPONENT_OFFSET_VAR].AddMember(JSON_COLLIDER_COMPONENT_X_VAR, m_vOffset.x, a_Allocator);
			a_Document[JSON_COLLIDER_COMPONENT_OFFSET_VAR].AddMember(JSON_COLLIDER_COMPONENT_Y_VAR, m_vOffset.y, a_Allocator);

			a_Document.AddMember(JSON_COLLIDER_COMPONENT_SIZE_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_COLLIDER_COMPONENT_SIZE_VAR].AddMember(JSON_COLLIDER_COMPONENT_X_VAR, m_vSize.x, a_Allocator);
			a_Document[JSON_COLLIDER_COMPONENT_SIZE_VAR].AddMember(JSON_COLLIDER_COMPONENT_Y_VAR, m_vSize.y, a_Allocator);
		}

		//---------------------------------------------------------------------
		void ColliderComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_vOffset = a_SrcData.GetVector(JSON_COLLIDER_COMPONENT_OFFSET_VAR);
			m_vSize = a_SrcData.GetVector(JSON_COLLIDER_COMPONENT_SIZE_VAR);
		}

		//---------------------------------------------------------------------
		void ColliderComponent::SetOffset(const DirectX::XMFLOAT2& a_vOffset)
		{
			m_vOffset = a_vOffset;
		}

		//---------------------------------------------------------------------
		void ColliderComponent::SetSize(const DirectX::XMFLOAT2& a_vSize)
		{
			m_vSize = a_vSize;
		}

		//---------------------------------------------------------------------
		const DirectX::XMFLOAT2& ColliderComponent::GetOffset() const
		{
			return m_vOffset;
		}

		//---------------------------------------------------------------------
		const DirectX::XMFLOAT2& ColliderComponent::GetSize() const
		{
			return m_vSize;
		}

		//---------------------------------------------------------------------
		void ColliderComponent::UpdateRealtime(float a_fDeltatime, UpdateTime a_UpdateTime)
		{
			if (a_UpdateTime == UpdateTime::UPDATE_TIME_END_FRAME)
			{
				MovementSystem& movementSys = core::ENGINE->GetECS().GetSystem<MovementSystem>();
				TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
				CollisionSystem& colliderSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();

				// both components must exist
				if (!movementSys.HasComponent(m_EntityID) || !transformSys.HasComponent(m_EntityID))
				{
					return;
				}

				MovementComponent& movementComp = movementSys.GetComponent(m_EntityID);
				TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);

				DirectX::XMFLOAT2 position = transformComp.Transform().GetPosition();
				DirectX::XMFLOAT2 scale = transformComp.Transform().GetScale();
				DirectX::XMFLOAT2 pivot = transformComp.Transform().GetPivot();
				float rotation = transformComp.Transform().GetRotation();

				DirectX::XMFLOAT2 move = movementComp.GetTranslation();

				float allowedMoveX = move.x;
				float allowedMoveY = move.y;

				auto& colliders = colliderSys.GetComponents();

				// Build player's AABB
				auto playerCorners = GetColliderWorldCorners(position, scale, pivot, rotation);
				float minX = playerCorners[0].x;
				float maxX = playerCorners[0].x;
				float minY = playerCorners[0].y;
				float maxY = playerCorners[0].y;

				for (int i = 1; i < 4; ++i)
				{
					if (playerCorners[i].x < minX) { minX = playerCorners[i].x; }
					if (playerCorners[i].x > maxX) { maxX = playerCorners[i].x; }
					if (playerCorners[i].y < minY) { minY = playerCorners[i].y; }
					if (playerCorners[i].y > maxY) { maxY = playerCorners[i].y; }
				}

				for (auto& m_ColliderPair : colliders)
				{
					if (m_ColliderPair.first == m_EntityID)
					{
						continue;
					}

					if (!transformSys.HasComponent(m_ColliderPair.first))
					{
						continue;
					}

					ColliderComponent& otherCollider = m_ColliderPair.second;
					TransformComponent& otherTransform = transformSys.GetComponent(m_ColliderPair.first);

					DirectX::XMFLOAT2 otherPosition = otherTransform.Transform().GetPosition();
					DirectX::XMFLOAT2 otherScale = otherTransform.Transform().GetScale();
					DirectX::XMFLOAT2 otherPivot = otherTransform.Transform().GetPivot();
					float otherRotation = otherTransform.Transform().GetRotation();

					auto otherCorners = otherCollider.GetColliderWorldCorners(otherPosition, otherScale, otherPivot, otherRotation);
					float otherMinX = otherCorners[0].x;
					float otherMaxX = otherCorners[0].x;
					float otherMinY = otherCorners[0].y;
					float otherMaxY = otherCorners[0].y;

					for (int i = 1; i < 4; ++i)
					{
						if (otherCorners[i].x < otherMinX) { otherMinX = otherCorners[i].x; }
						if (otherCorners[i].x > otherMaxX) { otherMaxX = otherCorners[i].x; }
						if (otherCorners[i].y < otherMinY) { otherMinY = otherCorners[i].y; }
						if (otherCorners[i].y > otherMaxY) { otherMaxY = otherCorners[i].y; }
					}

					//---------------------------------------------------------------------
					// X-axis collision
					if (move.x > 0.0f)
					{
						float dist = otherMinX - maxX;
						if (dist >= 0.0f && dist < allowedMoveX)
						{
							if (maxY > otherMinY && minY < otherMaxY)
							{
								allowedMoveX = dist;
								colliderSys.Collide(*this, otherCollider);
							}
						}
					}
					else if (move.x < 0.0f)
					{
						float dist = otherMaxX - minX;
						if (dist <= 0.0f && dist > allowedMoveX)
						{
							if (maxY > otherMinY && minY < otherMaxY)
							{
								allowedMoveX = dist;
								colliderSys.Collide(*this, otherCollider);
							}
						}
					}

					//---------------------------------------------------------------------
					// Y-axis collision
					if (move.y > 0.0f)
					{
						float dist = otherMinY - maxY;
						if (dist >= 0.0f && dist < allowedMoveY)
						{
							if (maxX > otherMinX && minX < otherMaxX)
							{
								allowedMoveY = dist;
								colliderSys.Collide(*this, otherCollider);
							}
						}
					}
					else if (move.y < 0.0f)
					{
						float dist = otherMaxY - minY;
						if (dist <= 0.0f && dist > allowedMoveY)
						{
							if (maxX > otherMinX && minX < otherMaxX)
							{
								allowedMoveY = dist;
								colliderSys.Collide(*this, otherCollider);
							}
						}
					}
				}

				//---------------------------------------------------------------------
				// Update movement component translation (do not modify position)
				const float epsilon = 0.001f;

				float finalMoveX = allowedMoveX;
				float finalMoveY = allowedMoveY;

				if (finalMoveX != 0.0f)
				{
					finalMoveX -= std::copysign(epsilon, finalMoveX);
				}

				if (finalMoveY != 0.0f)
				{
					finalMoveY -= std::copysign(epsilon, finalMoveY);
				}

				movementComp.SetTranslation({ finalMoveX, finalMoveY });
			}
		}

		//---------------------------------------------------------------------
		std::array<DirectX::XMFLOAT2, 4> ColliderComponent::GetColliderWorldCorners(
			const DirectX::XMFLOAT2& a_vPos,
			const DirectX::XMFLOAT2& a_vScale,
			const DirectX::XMFLOAT2& a_vPivot,
			float a_fRotation)
		{
			DirectX::XMFLOAT2 localCenter = {
				-a_vPivot.x * a_vScale.x + m_vOffset.x * a_vScale.x,
				-a_vPivot.y * a_vScale.y + m_vOffset.y * a_vScale.y
			};

			DirectX::XMFLOAT2 finalSize = {
				m_vSize.x * a_vScale.x,
				m_vSize.y * a_vScale.y
			};

			float rotationRad = DirectX::XMConvertToRadians(a_fRotation);
			float cosR = cosf(rotationRad);
			float sinR = sinf(rotationRad);

			DirectX::XMFLOAT2 colliderCenter = {
				a_vPos.x + localCenter.x * cosR - localCenter.y * sinR,
				a_vPos.y + localCenter.x * sinR + localCenter.y * cosR
			};

			std::array<DirectX::XMFLOAT2, 4> corners = {
				DirectX::XMFLOAT2(-0.5f * finalSize.x, -0.5f * finalSize.y),
				DirectX::XMFLOAT2(0.5f * finalSize.x, -0.5f * finalSize.y),
				DirectX::XMFLOAT2(0.5f * finalSize.x, 0.5f * finalSize.y),
				DirectX::XMFLOAT2(-0.5f * finalSize.x, 0.5f * finalSize.y)
			};

			for (int i = 0; i < 4; ++i)
			{
				float x = corners[i].x;
				float y = corners[i].y;
				corners[i].x = x * cosR - y * sinR + colliderCenter.x;
				corners[i].y = x * sinR + y * cosR + colliderCenter.y;
			}

			return corners;
		}

		//---------------------------------------------------------------------
		std::array<DirectX::XMFLOAT2, 4> ColliderComponent::GetAxes(const std::array<DirectX::XMFLOAT2, 4>& corners)
		{
			std::array<DirectX::XMFLOAT2, 2> axes; // only 2 unique axes for rectangle
			for (int i = 0; i < 2; ++i)
			{
				int next = (i + 1) % 4;
				float dx = corners[next].x - corners[i].x;
				float dy = corners[next].y - corners[i].y;
				// perpendicular (normal)
				float length = sqrtf(dx * dx + dy * dy);
				axes[i] = { -dy / length, dx / length };
			}
			return { axes[0], axes[1], axes[0], axes[1] }; // duplicate for simplicity
		}

		//---------------------------------------------------------------------
		bool ColliderComponent::OverlapsOnAxis(const std::array<DirectX::XMFLOAT2, 4>& a,
			const std::array<DirectX::XMFLOAT2, 4>& b,
			const DirectX::XMFLOAT2& axis)
		{
			float minA = FLT_MAX, maxA = -FLT_MAX;
			float minB = FLT_MAX, maxB = -FLT_MAX;

			for (auto& p : a) { float proj = p.x * axis.x + p.y * axis.y; minA = std::min(minA, proj); maxA = std::max(maxA, proj); }
			for (auto& p : b) { float proj = p.x * axis.x + p.y * axis.y; minB = std::min(minB, proj); maxB = std::max(maxB, proj); }

			return !(maxA < minB || maxB < minA);
		}

		//---------------------------------------------------------------------
		bool ColliderComponent::CheckCollision(
			ColliderComponent& a, const DirectX::XMFLOAT2& posA, const DirectX::XMFLOAT2& scaleA, const DirectX::XMFLOAT2& pivotA, float rotA,
			ColliderComponent& b, const DirectX::XMFLOAT2& posB, const DirectX::XMFLOAT2& scaleB, const DirectX::XMFLOAT2& pivotB, float rotB)
		{
			for (const auto& entityID : a.m_aEntitiesToIgnore)
			{
				if (entityID == a.GetEntityID() || entityID == b.GetEntityID())
				{
					return false;
				}
			}

			for (const auto& entityID : b.m_aEntitiesToIgnore)
			{
				if (entityID == a.GetEntityID() || entityID == b.GetEntityID())
				{
					return false;
				}
			}

			auto cornersA = a.GetColliderWorldCorners(posA, scaleA, pivotA, rotA);
			auto cornersB = b.GetColliderWorldCorners(posB, scaleB, pivotB, rotB);

			auto axesA = a.GetAxes(cornersA);
			auto axesB = b.GetAxes(cornersB);

			for (auto& axis : axesA) if (!OverlapsOnAxis(cornersA, cornersB, axis)) return false;
			for (auto& axis : axesB) if (!OverlapsOnAxis(cornersA, cornersB, axis)) return false;

			return true; 
		}

		//---------------------------------------------------------------------
		void ColliderComponent::IgnoreEntity(const gameplay::EntityID& a_EntityID)
		{
			m_aEntitiesToIgnore.insert(a_EntityID);
		}

		//---------------------------------------------------------------------
		bool ColliderComponent::IsPointInside(
			const DirectX::XMFLOAT2& a_vPoint,
			const DirectX::XMFLOAT2& a_vPos,
			const DirectX::XMFLOAT2& a_vScale,
			const DirectX::XMFLOAT2& a_vPivot,
			float a_fRotation)
		{
			std::array<DirectX::XMFLOAT2, 4> corners = GetColliderWorldCorners(a_vPos, a_vScale, a_vPivot, a_fRotation);

			float minX = corners[0].x;
			float maxX = corners[0].x;
			float minY = corners[0].y;
			float maxY = corners[0].y;

			for (int i = 1; i < 4; ++i)
			{
				if (corners[i].x < minX)
				{
					minX = corners[i].x;
				}
				if (corners[i].x > maxX)
				{
					maxX = corners[i].x;
				}
				if (corners[i].y < minY)
				{
					minY = corners[i].y;
				}
				if (corners[i].y > maxY)
				{
					maxY = corners[i].y;
				}
			}

			return (a_vPoint.x >= minX && a_vPoint.x <= maxX &&
				a_vPoint.y >= minY && a_vPoint.y <= maxY);
		}

		//---------------------------------------------------------------------
		float ColliderComponent::GetDistanceTo(
			const DirectX::XMFLOAT2& a_vPoint,
			const DirectX::XMFLOAT2& a_vPos,
			const DirectX::XMFLOAT2& a_vScale,
			const DirectX::XMFLOAT2& a_vPivot,
			float a_fRotation)
		{
			std::array<DirectX::XMFLOAT2, 4> corners = GetColliderWorldCorners(a_vPos, a_vScale, a_vPivot, a_fRotation);

			float minX = corners[0].x;
			float maxX = corners[0].x;
			float minY = corners[0].y;
			float maxY = corners[0].y;

			for (int i = 1; i < 4; ++i)
			{
				if (corners[i].x < minX)
				{
					minX = corners[i].x;
				}
				if (corners[i].x > maxX)
				{
					maxX = corners[i].x;
				}
				if (corners[i].y < minY)
				{
					minY = corners[i].y;
				}
				if (corners[i].y > maxY)
				{
					maxY = corners[i].y;
				}
			}

			float dx = 0.0f;
			float dy = 0.0f;

			if (a_vPoint.x < minX)
			{
				dx = minX - a_vPoint.x;
			}
			else if (a_vPoint.x > maxX)
			{
				dx = a_vPoint.x - maxX;
			}

			if (a_vPoint.y < minY)
			{
				dy = minY - a_vPoint.y;
			}
			else if (a_vPoint.y > maxY)
			{
				dy = a_vPoint.y - maxY;
			}

			return sqrtf(dx * dx + dy * dy);
		}
	}
}