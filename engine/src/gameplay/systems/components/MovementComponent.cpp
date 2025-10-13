#include "gameplay/systems/components/MovementComponent.h"

#include <rapidjson/utils.h>

#include "resources/SrcData.h"

#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/CollisionSystem.h"

#define JSON_MOVEMENT_COMPONENT_GRAVITY_VAR "gravity"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// MovementComponent
		//---------------------------------------------------------------------
		void MovementComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_MOVEMENT_COMPONENT_GRAVITY_VAR, m_bHasGravity, a_Allocator);
		}

        //---------------------------------------------------------------------
        void MovementComponent::Deserialize(const resources::SrcData& a_SrcData)
        {
            m_bHasGravity = a_SrcData.GetBool(JSON_MOVEMENT_COMPONENT_GRAVITY_VAR);
        }

		//---------------------------------------------------------------------
		void MovementComponent::Translate(const DirectX::XMFLOAT2& a_vTranslation)
		{
            m_vTranslation = {
                m_vTranslation.x + a_vTranslation.x,
                m_vTranslation.y + a_vTranslation.y
            };
		}

        //---------------------------------------------------------------------
        //---------------------------------------------------------------------
        void MovementComponent::UpdateRealtime(float a_fDeltaTime, std::map<EntityID, ColliderComponent>& a_aColliders)
        {
            if (m_bHasGravity)
            {
                m_vTranslation.y += 20.0f;
            }

            TransformSystem& m_TransformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

            const Entity* m_pEntity = core::ENGINE->GetECS().GetEntity(m_EntityID);
            if (m_pEntity == nullptr)
            {
                return;
            }

            TransformComponent& m_TransformComp = m_TransformSys.GetComponent(m_pEntity->GetEntityID());
            DirectX::XMFLOAT2 m_vCurrentPos = m_TransformComp.Transform().GetPosition();
            DirectX::XMFLOAT2 m_vScale = m_TransformComp.Transform().GetScale();
            DirectX::XMFLOAT2 m_vPivot = m_TransformComp.Transform().GetPivot();
            float m_fRotation = m_TransformComp.Transform().GetRotation();

            DirectX::XMFLOAT2 m_vNewPos = m_vCurrentPos;

            if (!a_aColliders.contains(m_EntityID))
            {
                m_TransformComp.Transform().SetPosition(m_vNewPos);
                return;
            }

            ColliderComponent& m_PlayerCollider = a_aColliders[m_EntityID];

            // Start with full desired movement
            float m_fAllowedMoveX = m_vTranslation.x;
            float m_fAllowedMoveY = m_vTranslation.y;

            // Build player's AABB from its world corners (current position)
            auto m_PlayerCorners = m_PlayerCollider.GetColliderWorldCorners(m_vCurrentPos, m_vScale, m_vPivot, m_fRotation);
            float m_pMinX = m_PlayerCorners[0].x;
            float m_pMaxX = m_PlayerCorners[0].x;
            float m_pMinY = m_PlayerCorners[0].y;
            float m_pMaxY = m_PlayerCorners[0].y;

            for (int i = 1; i < 4; ++i)
            {
                if (m_PlayerCorners[i].x < m_pMinX) { m_pMinX = m_PlayerCorners[i].x; }
                if (m_PlayerCorners[i].x > m_pMaxX) { m_pMaxX = m_PlayerCorners[i].x; }
                if (m_PlayerCorners[i].y < m_pMinY) { m_pMinY = m_PlayerCorners[i].y; }
                if (m_PlayerCorners[i].y > m_pMaxY) { m_pMaxY = m_PlayerCorners[i].y; }
            }

            for (auto& m_ColliderPair : a_aColliders)
            {
                if (m_ColliderPair.first == m_EntityID)
                {
                    continue;
                }

                if (!m_TransformSys.HasComponent(m_ColliderPair.first))
                {
                    continue;
                }

                ColliderComponent& m_OtherCollider = m_ColliderPair.second;
                TransformComponent& m_OtherTransform = m_TransformSys.GetComponent(m_ColliderPair.first);

                DirectX::XMFLOAT2 m_vOtherPos = m_OtherTransform.Transform().GetPosition();
                DirectX::XMFLOAT2 m_vOtherScale = m_OtherTransform.Transform().GetScale();
                DirectX::XMFLOAT2 m_vOtherPivot = m_OtherTransform.Transform().GetPivot();
                float m_fOtherRotation = m_OtherTransform.Transform().GetRotation();

                // Build other AABB from its world corners
                auto m_OtherCorners = m_OtherCollider.GetColliderWorldCorners(m_vOtherPos, m_vOtherScale, m_vOtherPivot, m_fOtherRotation);
                float m_oMinX = m_OtherCorners[0].x;
                float m_oMaxX = m_OtherCorners[0].x;
                float m_oMinY = m_OtherCorners[0].y;
                float m_oMaxY = m_OtherCorners[0].y;

                for (int i = 1; i < 4; ++i)
                {
                    if (m_OtherCorners[i].x < m_oMinX) { m_oMinX = m_OtherCorners[i].x; }
                    if (m_OtherCorners[i].x > m_oMaxX) { m_oMaxX = m_OtherCorners[i].x; }
                    if (m_OtherCorners[i].y < m_oMinY) { m_oMinY = m_OtherCorners[i].y; }
                    if (m_OtherCorners[i].y > m_oMaxY) { m_oMaxY = m_OtherCorners[i].y; }
                }

                //---------------------------------------------------------------------
                // X-axis distance check (moving right)
                if (m_vTranslation.x > 0.0f)
                {
                    float m_fDist = m_oMinX - m_pMaxX; // positive = gap on right
                    if (m_fDist >= 0.0f && m_fDist < m_fAllowedMoveX)
                    {
                        // Only treat it as blocking if Y ranges overlap
                        if (m_pMaxY > m_oMinY && m_pMinY < m_oMaxY)
                        {
                            m_fAllowedMoveX = m_fDist;
                            core::ENGINE->GetECS().GetSystem<CollisionSystem>().Collide(m_PlayerCollider, m_OtherCollider);
                        }
                    }
                }
                else if (m_vTranslation.x < 0.0f)
                {
                    // moving left
                    float m_fDist = m_oMaxX - m_pMinX; // negative or zero
                    if (m_fDist <= 0.0f && m_fDist > m_fAllowedMoveX)
                    {
                        if (m_pMaxY > m_oMinY && m_pMinY < m_oMaxY)
                        {
                            m_fAllowedMoveX = m_fDist;
                            core::ENGINE->GetECS().GetSystem<CollisionSystem>().Collide(m_PlayerCollider, m_OtherCollider);
                        }
                    }
                }

                //---------------------------------------------------------------------
                // Y-axis distance check (moving down = positive Y in your code)
                if (m_vTranslation.y > 0.0f)
                {
                    float m_fDist = m_oMinY - m_pMaxY; // positive = gap below
                    if (m_fDist >= 0.0f && m_fDist < m_fAllowedMoveY)
                    {
                        // Only treat it as blocking if X ranges overlap
                        if (m_pMaxX > m_oMinX && m_pMinX < m_oMaxX)
                        {
                            m_fAllowedMoveY = m_fDist;
                            core::ENGINE->GetECS().GetSystem<CollisionSystem>().Collide(m_PlayerCollider, m_OtherCollider);
                        }
                    }
                }
                else if (m_vTranslation.y < 0.0f)
                {
                    // moving up (negative Y)
                    float m_fDist = m_oMaxY - m_pMinY; // negative or zero
                    if (m_fDist <= 0.0f && m_fDist > m_fAllowedMoveY)
                    {
                        if (m_pMaxX > m_oMinX && m_pMinX < m_oMaxX)
                        {
                            m_fAllowedMoveY = m_fDist;
                            core::ENGINE->GetECS().GetSystem<CollisionSystem>().Collide(m_PlayerCollider, m_OtherCollider);
                        }
                    }
                }
            }

            //---------------------------------------------------------------------
            // Move as far as possible without collision (small epsilon to avoid sticking)
            const float m_fEpsilon = 0.001f;

            if (fabs(m_fAllowedMoveX) > 0.0f)
            {
                m_vNewPos.x += m_fAllowedMoveX - std::copysign(m_fEpsilon, m_fAllowedMoveX);
            }

            if (fabs(m_fAllowedMoveY) > 0.0f)
            {
                m_vNewPos.y += m_fAllowedMoveY - std::copysign(m_fEpsilon, m_fAllowedMoveY);
            }

            m_TransformComp.Transform().SetPosition(m_vNewPos);

            // Reset translation applied this frame
            m_vTranslation = {};
        }

	}
}