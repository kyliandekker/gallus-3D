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
        void MovementComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
        {
			const Entity* entity = core::ENGINE->GetECS().GetEntity(m_EntityID);
			if (entity == nullptr)
			{
				return;
			}

			if (!entity->IsActive())
			{
				return;
			}

			if (a_UpdateTime == UpdateTime::UPDATE_TIME_FRAME)
			{
				if (m_bHasGravity)
				{
					m_vTranslation.y += 20.0f;
				}
			}
			if (a_UpdateTime == UpdateTime::UPDATE_TIME_POST_FRAME)
			{
				if (m_vTranslation.x == 0.0f && m_vTranslation.y == 0.0)
				{
					return;
				}

				TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

				TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);
				DirectX::XMFLOAT2 newPos = {
					 transformComp.Transform().GetPosition().x + m_vTranslation.x,
					 transformComp.Transform().GetPosition().y + m_vTranslation.y,
				};
				transformComp.Transform().SetPosition(newPos);

				m_vTranslation = {};
			}
        }

	}
}