#include "gameplay/systems/components/ColliderComponent.h"

#include <rapidjson/utils.h>

#include "resources/SrcData.h"

#include "core/Engine.h"

#include "gameplay/Entity.h"
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
			if (a_UpdateTime == UpdateTime::UPDATE_TIME_FRAME_RESOLVE)
			{
				TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();
				CollisionSystem& colliderSys = core::ENGINE->GetECS().GetSystem<CollisionSystem>();

				// both components must exist
				if (!transformSys.HasComponent(m_EntityID))
				{
					return;
				}

				TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);

				DirectX::XMFLOAT2 position = transformComp.Transform().GetPosition();
				DirectX::XMFLOAT2 scale = transformComp.Transform().GetScale();
				DirectX::XMFLOAT2 pivot = transformComp.Transform().GetPivot();
				float rotation = transformComp.Transform().GetRotation();

				DirectX::XMFLOAT2 move = transformComp.GetTranslation();

				float allowedMoveX = move.x;
				float allowedMoveY = move.y;

				auto& colliders = colliderSys.GetComponents();

				// Build player's rotated corners (current)
				auto playerCorners = GetColliderWorldCorners(position, scale, pivot, rotation);

				//---------------------------------------------------------------------
				// Helper lambdas (SAT with correct perpendicular axes)
				auto Normalize = [](const DirectX::XMFLOAT2& v) -> DirectX::XMFLOAT2
				{
					float len = sqrtf(v.x * v.x + v.y * v.y);
					if (len == 0.0f) { return { 0.0f, 0.0f }; }
					return { v.x / len, v.y / len };
				};

				auto Project = [](const std::array<DirectX::XMFLOAT2, 4>& corners, const DirectX::XMFLOAT2& axis, float& outMin, float& outMax)
				{
					outMin = outMax = corners[0].x * axis.x + corners[0].y * axis.y;
					for (int i = 1; i < 4; ++i)
					{
						float proj = corners[i].x * axis.x + corners[i].y * axis.y;
						if (proj < outMin) outMin = proj;
						if (proj > outMax) outMax = proj;
					}
				};

				auto GetAxesPerp = [&](const std::array<DirectX::XMFLOAT2, 4>& corners)
				{
					// Two unique edges: (1-0) and (3-0); axes are perpendicular to these edges
					std::array<DirectX::XMFLOAT2, 2> axes;
					DirectX::XMFLOAT2 e0 = { corners[1].x - corners[0].x, corners[1].y - corners[0].y };
					DirectX::XMFLOAT2 e1 = { corners[3].x - corners[0].x, corners[3].y - corners[0].y };
					// perpendicular: (-y, x)
					axes[0] = Normalize({ -e0.y, e0.x });
					axes[1] = Normalize({ -e1.y, e1.x });
					return axes;
				};

				auto SATOverlap = [&](const std::array<DirectX::XMFLOAT2, 4>& a, const std::array<DirectX::XMFLOAT2, 4>& b) -> bool
				{
					auto axesA = GetAxesPerp(a);
					auto axesB = GetAxesPerp(b);

					float minA, maxA, minB, maxB;

					for (int i = 0; i < 2; ++i)
					{
						Project(a, axesA[i], minA, maxA);
						Project(b, axesA[i], minB, maxB);
						if (maxA < minB || maxB < minA) return false;
					}

					for (int i = 0; i < 2; ++i)
					{
						Project(a, axesB[i], minA, maxA);
						Project(b, axesB[i], minB, maxB);
						if (maxA < minB || maxB < minA) return false;
					}

					return true;
				};

				//---------------------------------------------------------------------
				for (auto& colliderPair : colliders)
				{
					if (colliderPair.first == m_EntityID) { continue; }
					if (!transformSys.HasComponent(colliderPair.first)) { continue; }

					ColliderComponent& otherCollider = colliderPair.second;
					TransformComponent& otherTransform = transformSys.GetComponent(colliderPair.first);

					DirectX::XMFLOAT2 otherPosition = otherTransform.Transform().GetPosition();
					DirectX::XMFLOAT2 otherScale = otherTransform.Transform().GetScale();
					DirectX::XMFLOAT2 otherPivot = otherTransform.Transform().GetPivot();
					float otherRotation = otherTransform.Transform().GetRotation();

					auto otherCorners = otherCollider.GetColliderWorldCorners(otherPosition, otherScale, otherPivot, otherRotation);

					// Accumulate normal components for this pair
					DirectX::XMFLOAT2 accumulatedNormal = { 0.0f, 0.0f };

					//-----------------------------------------------------------------
					// Check X movement: build player corners moved only in X and test
					if (move.x != 0.0f)
					{
						std::array<DirectX::XMFLOAT2, 4> movedX = playerCorners;
						for (int i = 0; i < 4; ++i) { movedX[i].x += move.x; }

						if (SATOverlap(movedX, otherCorners))
						{
							// binary search maximal safe delta along X
							float low = 0.0f;
							float high = move.x;
							// ensure correct ordering for negative moves
							for (int it = 0; it < 8; ++it)
							{
								float mid = (low + high) * 0.5f;
								std::array<DirectX::XMFLOAT2, 4> test = playerCorners;
								for (int i = 0; i < 4; ++i) { test[i].x += mid; }

								if (SATOverlap(test, otherCorners))
								{
									// mid penetrates -> move high toward mid (reduce magnitude)
									if (move.x > 0.0f) high = mid;
									else low = mid;
								}
								else
								{
									// mid safe -> move low toward mid (expand)
									if (move.x > 0.0f) low = mid;
									else high = mid;
								}
							}

							float safeDeltaX = (move.x > 0.0f) ? low : high;
							if ((move.x > 0.0f && safeDeltaX < allowedMoveX) || (move.x < 0.0f && safeDeltaX > allowedMoveX))
							{
								allowedMoveX = safeDeltaX;
							}

							// X normal contribution (pointing away from other)
							accumulatedNormal.x = (move.x > 0.0f) ? 1.0f : -1.0f;
						}
					}

					//-----------------------------------------------------------------
					// Check Y movement: build player corners moved only in Y and test
					if (move.y != 0.0f)
					{
						std::array<DirectX::XMFLOAT2, 4> movedY = playerCorners;
						for (int i = 0; i < 4; ++i) { movedY[i].y += move.y; }

						if (SATOverlap(movedY, otherCorners))
						{
							// binary search maximal safe delta along Y
							float low = 0.0f;
							float high = move.y;
							for (int it = 0; it < 8; ++it)
							{
								float mid = (low + high) * 0.5f;
								std::array<DirectX::XMFLOAT2, 4> test = playerCorners;
								for (int i = 0; i < 4; ++i) { test[i].y += mid; }

								if (SATOverlap(test, otherCorners))
								{
									if (move.y > 0.0f) high = mid;
									else low = mid;
								}
								else
								{
									if (move.y > 0.0f) low = mid;
									else high = mid;
								}
							}

							float safeDeltaY = (move.y > 0.0f) ? low : high;
							if ((move.y > 0.0f && safeDeltaY < allowedMoveY) || (move.y < 0.0f && safeDeltaY > allowedMoveY))
							{
								allowedMoveY = safeDeltaY;
							}

							accumulatedNormal.y = (move.y > 0.0f) ? 1.0f : -1.0f;
						}
					}

					//-----------------------------------------------------------------
					// If we found any collision contribution, send combined normalized normal once
					if (accumulatedNormal.x != 0.0f || accumulatedNormal.y != 0.0f)
					{
						float len = sqrtf(accumulatedNormal.x * accumulatedNormal.x + accumulatedNormal.y * accumulatedNormal.y);
						DirectX::XMFLOAT2 normalToSend = { 0.0f, 0.0f };
						if (len > 0.0f)
						{
							normalToSend.x = accumulatedNormal.x / len;
							normalToSend.y = accumulatedNormal.y / len;
						}

						colliderSys.Collide(*this, otherCollider, normalToSend);
					}
				}

				//---------------------------------------------------------------------
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

				transformComp.SetTranslation({ finalMoveX, finalMoveY });
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

		bool ColliderComponent::IntersectsOBB(const std::array<DirectX::XMFLOAT2, 4>& aA,
			const std::array<DirectX::XMFLOAT2, 4>& aB)
		{
			auto Project = [](const std::array<DirectX::XMFLOAT2, 4>& corners, const DirectX::XMFLOAT2& axis, float& outMin, float& outMax)
			{
				outMin = outMax = corners[0].x * axis.x + corners[0].y * axis.y;
				for (int i = 1; i < 4; ++i)
				{
					float projection = corners[i].x * axis.x + corners[i].y * axis.y;
					if (projection < outMin) { outMin = projection; }
					if (projection > outMax) { outMax = projection; }
				}
			};

			auto Normalize = [](const DirectX::XMFLOAT2& v)
			{
				float len = sqrtf(v.x * v.x + v.y * v.y);
				return DirectX::XMFLOAT2(v.x / len, v.y / len);
			};

			std::array<DirectX::XMFLOAT2, 4> axes = {
				Normalize({ aA[1].x - aA[0].x, aA[1].y - aA[0].y }),
				Normalize({ aA[3].x - aA[0].x, aA[3].y - aA[0].y }),
				Normalize({ aB[1].x - aB[0].x, aB[1].y - aB[0].y }),
				Normalize({ aB[3].x - aB[0].x, aB[3].y - aB[0].y }),
			};

			for (auto& axis : axes)
			{
				float minA, maxA, minB, maxB;
				Project(aA, axis, minA, maxA);
				Project(aB, axis, minB, maxB);

				if (maxA < minB || maxB < minA)
				{
					return false; // Gap found → no collision
				}
			}

			return true; // Overlaps on all axes
		}

		//---------------------------------------------------------------------
		void ColliderComponent::IgnoreEntity(const gameplay::EntityID& a_EntityID)
		{
			m_aEntitiesToIgnore.insert(a_EntityID);
		}
	}
}