#include "gameplay/systems/components/MovementComponent.h"

#include <rapidjson/utils.h>

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
		void MovementComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			rapidjson::GetBool(a_Document, JSON_MOVEMENT_COMPONENT_GRAVITY_VAR, m_bHasGravity);
		}

		//---------------------------------------------------------------------
		void MovementComponent::Translate(const DirectX::XMFLOAT3& a_vTranslation)
		{
            m_vTranslation = {
                m_vTranslation.x + a_vTranslation.x,
                m_vTranslation.y + a_vTranslation.y,
                m_vTranslation.z + a_vTranslation.z
            };
		}

        //---------------------------------------------------------------------
        void MovementComponent::UpdateRealtime(float a_fDeltaTime, std::map<EntityID, ColliderComponent>& a_aColliders)
        {
            if (m_bHasGravity)
            {
                m_vTranslation.y += 20;
            }

            TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

            const Entity* ent = core::ENGINE->GetECS().GetEntity(m_EntityID);
            if (ent == nullptr)
            {
                return;
            }

            TransformComponent& transformComp = transformSys.GetComponent(ent->GetEntityID());
            DirectX::XMFLOAT3 currentPos = transformComp.Transform().GetPosition();
            DirectX::XMFLOAT3 newPos = currentPos;

            if (!a_aColliders.contains(m_EntityID))
            {
                transformComp.Transform().SetPosition(newPos);
                return;
            }

            auto& playerCollider = a_aColliders[m_EntityID];

            bool allowX = true;
            bool allowY = true;

            for (auto& colliderPair : a_aColliders)
            {
                if (colliderPair.first == m_EntityID)
                {
                    continue;
                }

                if (!transformSys.HasComponent(colliderPair.first))
                {
                    continue;
                }

                auto& otherCollider = colliderPair.second;
                TransformComponent& otherTransform = transformSys.GetComponent(colliderPair.first);

                // Test X movement
                //if (allowX)
                //{
                //    DirectX::XMFLOAT2 testPosX = { newPos.x + m_vTranslation.x, newPos.y };
                //    if (ColliderComponent::CheckCollision(
                //        playerCollider, testPosX, transformComp.Transform().GetScale(), transformComp.Transform().GetPivot(), transformComp.Transform().GetRotation(),
                //        otherCollider, otherTransform.Transform().GetPosition(), otherTransform.Transform().GetScale(), otherTransform.Transform().GetPivot(), otherTransform.Transform().GetRotation()))
                //    {
                //        core::ENGINE->GetECS().GetSystem<CollisionSystem>().Collide(playerCollider, otherCollider);
                //        allowX = false;
                //    }
                //}

                //// Test Y movement
                //if (allowY)
                //{
                //    DirectX::XMFLOAT2 testPosY = { newPos.x, newPos.y + m_vTranslation.y };
                //    if (ColliderComponent::CheckCollision(
                //        playerCollider, testPosY, transformComp.Transform().GetScale(), transformComp.Transform().GetPivot(), transformComp.Transform().GetRotation(),
                //        otherCollider, otherTransform.Transform().GetPosition(), otherTransform.Transform().GetScale(), otherTransform.Transform().GetPivot(), otherTransform.Transform().GetRotation()))
                //    {
                //        core::ENGINE->GetECS().GetSystem<CollisionSystem>().Collide(playerCollider, otherCollider);
                //        allowY = false;
                //    }
                //}

                if (!allowX && !allowY)
                {
                    break; // no movement possible, exit early
                }
            }

            if (allowX)
            {
                newPos.x += m_vTranslation.x;
            }
            if (allowY)
            {
                newPos.y += m_vTranslation.y;
            }

            transformComp.Transform().SetPosition(newPos);

            m_vTranslation = {};
        }
	}
}