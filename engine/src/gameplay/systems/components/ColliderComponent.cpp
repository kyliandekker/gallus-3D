#include "gameplay/systems/components/ColliderComponent.h"

// core
#include "core/Engine.h"

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/Entity.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/CollisionSystem.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// ColliderComponent
		//---------------------------------------------------------------------
		void ColliderComponent::UpdateRealtime(float a_fDeltatime, UpdateTime a_UpdateTime)
		{
			//if (a_UpdateTime == UpdateTime::UPDATE_TIME_FRAME_RESOLVE)
			//{
			//	TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
			//	CollisionSystem& colliderSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();

			//	if (!transformSys.HasComponent(m_EntityID))
			//	{
			//		return;
			//	}

			//	TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);

			//	DirectX::XMFLOAT2 position = transformComp.GetTransform().GetPosition();
			//	DirectX::XMFLOAT2 scale = transformComp.GetTransform().GetScale();
			//	DirectX::XMFLOAT2 pivot = transformComp.GetTransform().GetPivot();
			//	float rotation = transformComp.GetTransform().GetRotation();

			//	DirectX::XMFLOAT2 move = transformComp.GetTranslation();

			//	float allowedMoveX = move.x;
			//	float allowedMoveY = move.y;

			//	auto& colliders = colliderSys.GetComponents();

			//	auto playerCorners = GetColliderWorldCorners(position, scale, pivot, rotation);

			//	auto Normalize = [](const DirectX::XMFLOAT2& v) -> DirectX::XMFLOAT2
			//		{
			//			float len = sqrtf(v.x * v.x + v.y * v.y);
			//			if (len == 0.0f)
			//			{
			//				return { 0.0f, 0.0f };
			//			}
			//			return { v.x / len, v.y / len };
			//		};

			//	auto Project = [](const std::array<DirectX::XMFLOAT2, 4>& corners, const DirectX::XMFLOAT2& axis, float& outMin, float& outMax)
			//		{
			//			outMin = outMax = corners[0].x * axis.x + corners[0].y * axis.y;
			//			for (int i = 1; i < 4; ++i)
			//			{
			//				float proj = corners[i].x * axis.x + corners[i].y * axis.y;
			//				if (proj < outMin) { outMin = proj; }
			//				if (proj > outMax) { outMax = proj; }
			//			}
			//		};

			//	auto GetAxesPerp = [&](const std::array<DirectX::XMFLOAT2, 4>& corners)
			//		{
			//			std::array<DirectX::XMFLOAT2, 2> axes;
			//			DirectX::XMFLOAT2 e0 = { corners[1].x - corners[0].x, corners[1].y - corners[0].y };
			//			DirectX::XMFLOAT2 e1 = { corners[3].x - corners[0].x, corners[3].y - corners[0].y };
			//			axes[0] = Normalize({ -e0.y, e0.x });
			//			axes[1] = Normalize({ -e1.y, e1.x });
			//			return axes;
			//		};

			//	auto SATOverlap = [&](const std::array<DirectX::XMFLOAT2, 4>& a, const std::array<DirectX::XMFLOAT2, 4>& b) -> bool
			//		{
			//			auto axesA = GetAxesPerp(a);
			//			auto axesB = GetAxesPerp(b);
			//			float minA, maxA, minB, maxB;
			//			for (int i = 0; i < 2; ++i)
			//			{
			//				if (axesA[i].x == 0.0f && axesA[i].y == 0.0f) { continue; }
			//				Project(a, axesA[i], minA, maxA);
			//				Project(b, axesA[i], minB, maxB);
			//				if (maxA < minB || maxB < minA) { return false; }
			//			}
			//			for (int i = 0; i < 2; ++i)
			//			{
			//				if (axesB[i].x == 0.0f && axesB[i].y == 0.0f) { continue; }
			//				Project(a, axesB[i], minA, maxA);
			//				Project(b, axesB[i], minB, maxB);
			//				if (maxA < minB || maxB < minA) { return false; }
			//			}
			//			return true;
			//		};

			//	auto OffsetCorners = [&](const std::array<DirectX::XMFLOAT2, 4>& src, float offset, bool isX)
			//		{
			//			std::array<DirectX::XMFLOAT2, 4> out = src;
			//			for (int i = 0; i < 4; ++i)
			//			{
			//				if (isX) { out[i].x += offset; }
			//				else { out[i].y += offset; }
			//			}
			//			return out;
			//		};

			//	auto ComputeSafeDelta = [&](float moveComponent, bool isX, const std::array<DirectX::XMFLOAT2, 4>& aPlayerCorners,
			//		const std::array<DirectX::XMFLOAT2, 4>& aOtherCorners) -> float
			//		{
			//			if (moveComponent == 0.0f)
			//			{
			//				return 0.0f;
			//			}

			//			float minBound = std::min(0.0f, moveComponent);
			//			float maxBound = std::max(0.0f, moveComponent);

			//			auto cornersAtMin = OffsetCorners(aPlayerCorners, minBound, isX);
			//			auto cornersAtMax = OffsetCorners(aPlayerCorners, maxBound, isX);

			//			bool minPen = SATOverlap(cornersAtMin, aOtherCorners);
			//			bool maxPen = SATOverlap(cornersAtMax, aOtherCorners);

			//			if (!minPen && !maxPen)
			//			{
			//				return moveComponent;
			//			}

			//			float safe = 0.0f;
			//			float pen = 0.0f;

			//			if (!minPen && maxPen)
			//			{
			//				safe = minBound;
			//				pen = maxBound;
			//			}
			//			else if (minPen && !maxPen)
			//			{
			//				safe = maxBound;
			//				pen = minBound;
			//			}
			//			else
			//			{
			//				safe = 0.0f;
			//				pen = moveComponent;
			//			}

			//			for (int it = 0; it < 12; ++it)
			//			{
			//				float mid = (safe + pen) * 0.5f;
			//				auto test = OffsetCorners(aPlayerCorners, mid, isX);
			//				if (SATOverlap(test, aOtherCorners))
			//				{
			//					pen = mid;
			//				}
			//				else
			//				{
			//					safe = mid;
			//				}
			//			}

			//			return safe;
			//		};

			//	for (auto& colliderPair : colliders)
			//	{
			//		if (colliderPair.first == m_EntityID) { continue; }
			//		if (m_aEntitiesToIgnore.contains(colliderPair.first)) { continue; }
			//		if (colliderPair.second.m_aEntitiesToIgnore.contains(m_EntityID)) { continue; }
			//		if (!transformSys.HasComponent(colliderPair.first)) { continue; }

			//		ColliderComponent& otherCollider = colliderPair.second;
			//		TransformComponent& otherTransform = transformSys.GetComponent(colliderPair.first);

			//		DirectX::XMFLOAT2 otherPosition = otherTransform.GetTransform().GetPosition();
			//		DirectX::XMFLOAT2 otherScale = otherTransform.GetTransform().GetScale();
			//		DirectX::XMFLOAT2 otherPivot = otherTransform.GetTransform().GetPivot();
			//		float otherRotation = otherTransform.GetTransform().GetRotation();

			//		auto otherCorners = otherCollider.GetColliderWorldCorners(otherPosition, otherScale, otherPivot, otherRotation);

			//		DirectX::XMFLOAT2 accumulatedNormal = { 0.0f, 0.0f };

			//		if (move.x != 0.0f)
			//		{
			//			std::array<DirectX::XMFLOAT2, 4> movedX = playerCorners;
			//			for (int i = 0; i < 4; ++i) { movedX[i].x += move.x; }

			//			if (SATOverlap(movedX, otherCorners))
			//			{
			//				float safeDeltaX = ComputeSafeDelta(move.x, true, playerCorners, otherCorners);
			//				if ((move.x > 0.0f && safeDeltaX < allowedMoveX) || (move.x < 0.0f && safeDeltaX > allowedMoveX))
			//				{
			//					allowedMoveX = safeDeltaX;
			//				}
			//				accumulatedNormal.x = (move.x > 0.0f) ? 1.0f : -1.0f;
			//			}
			//		}

			//		if (move.y != 0.0f)
			//		{
			//			std::array<DirectX::XMFLOAT2, 4> movedY = playerCorners;
			//			for (int i = 0; i < 4; ++i) { movedY[i].y += move.y; }

			//			if (SATOverlap(movedY, otherCorners))
			//			{
			//				float safeDeltaY = ComputeSafeDelta(move.y, false, playerCorners, otherCorners);
			//				if ((move.y > 0.0f && safeDeltaY < allowedMoveY) || (move.y < 0.0f && safeDeltaY > allowedMoveY))
			//				{
			//					allowedMoveY = safeDeltaY;
			//				}

			//				accumulatedNormal.y = (move.y > 0.0f) ? 1.0f : -1.0f;
			//			}
			//		}

			//		if (accumulatedNormal.x != 0.0f || accumulatedNormal.y != 0.0f)
			//		{
			//			float len = sqrtf(accumulatedNormal.x * accumulatedNormal.x + accumulatedNormal.y * accumulatedNormal.y);
			//			DirectX::XMFLOAT2 normalToSend = { 0.0f, 0.0f };
			//			if (len > 0.0f)
			//			{
			//				normalToSend.x = accumulatedNormal.x / len;
			//				normalToSend.y = accumulatedNormal.y / len;
			//			}

			//			// Flip Y normal because up is -1, down is +1
			//			normalToSend.y *= -1.0f;

			//			colliderSys.Collide(*this, otherCollider, normalToSend);
			//		}
			//	}

			//	const float epsilon = 0.001f;
			//	float finalMoveX = allowedMoveX;
			//	float finalMoveY = allowedMoveY;

			//	if (finalMoveX != 0.0f)
			//	{
			//		if (fabs(finalMoveX) <= epsilon) { finalMoveX = 0.0f; }
			//		else { finalMoveX -= std::copysign(epsilon, finalMoveX); }
			//	}

			//	if (finalMoveY != 0.0f)
			//	{
			//		if (fabs(finalMoveY) <= epsilon) { finalMoveY = 0.0f; }
			//		else { finalMoveY -= std::copysign(epsilon, finalMoveY); }
			//	}

			//	transformComp.SetTranslation({ finalMoveX, finalMoveY });
			//}
		}

		//----------ColliderComponent-----------------------------------------------------------
		void ColliderComponent::IgnoreEntity(const gameplay::EntityID& a_EntityID)
		{
			m_aEntitiesToIgnore.insert(a_EntityID);
		}
	}
}