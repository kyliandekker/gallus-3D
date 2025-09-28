#include "gameplay/systems/components/ColliderComponent.h"

#include <rapidjson/utils.h>

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
		void ColliderComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			if (a_Document.HasMember(JSON_COLLIDER_COMPONENT_OFFSET_VAR) && a_Document[JSON_COLLIDER_COMPONENT_OFFSET_VAR].IsObject())
			{
				rapidjson::GetFloat(a_Document[JSON_COLLIDER_COMPONENT_OFFSET_VAR], JSON_COLLIDER_COMPONENT_X_VAR, m_vOffset.x);
				rapidjson::GetFloat(a_Document[JSON_COLLIDER_COMPONENT_OFFSET_VAR], JSON_COLLIDER_COMPONENT_Y_VAR, m_vOffset.y);
			}

			if (a_Document.HasMember(JSON_COLLIDER_COMPONENT_SIZE_VAR) && a_Document[JSON_COLLIDER_COMPONENT_SIZE_VAR].IsObject())
			{
				rapidjson::GetFloat(a_Document[JSON_COLLIDER_COMPONENT_SIZE_VAR], JSON_COLLIDER_COMPONENT_X_VAR, m_vSize.x);
				rapidjson::GetFloat(a_Document[JSON_COLLIDER_COMPONENT_SIZE_VAR], JSON_COLLIDER_COMPONENT_Y_VAR, m_vSize.y);
			}
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
	}
}