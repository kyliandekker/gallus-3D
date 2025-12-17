#include "TransformComponent.h"

// external
#include <DirectXMath.h>

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/EntityID.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// TransformComponent
		//---------------------------------------------------------------------
		graphics::dx12::Transform& TransformComponent::GetTransform()
		{
			return m_Transform;
		}

		//---------------------------------------------------------------------
		void TransformComponent::Translate(const DirectX::XMFLOAT3& a_vTranslation)
		{
			m_vTranslation = {
				m_vTranslation.x + a_vTranslation.x,
				m_vTranslation.y + a_vTranslation.y,
				m_vTranslation.z + a_vTranslation.z
			};
		}

		//---------------------------------------------------------------------
		void TransformComponent::UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime)
		{
			if (m_vTranslation.x == 0.0f && m_vTranslation.y == 0.0)
			{
				return;
			}

			TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<TransformSystem>();

			TransformComponent& transformComp = transformSys.GetComponent(m_EntityID);
			DirectX::XMFLOAT3 newPos = {
				transformComp.GetTransform().GetPosition().x + m_vTranslation.x,
				transformComp.GetTransform().GetPosition().y + m_vTranslation.y,
				transformComp.GetTransform().GetPosition().z + m_vTranslation.z
			};
			transformComp.GetTransform().SetPosition(newPos);

			m_vTranslation = {};
		}
	}
}