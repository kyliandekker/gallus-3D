#include "TransformComponent.h"

// external
#include <rapidjson/utils.h>
#include <DirectXMath.h>

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/EntityID.h"

#define JSON_TRANSFORM_COMPONENT_POSITION_VAR "position"
#define JSON_TRANSFORM_COMPONENT_ROTATION_VAR "rotation"
#define JSON_TRANSFORM_COMPONENT_SCALE_VAR "scale"
#define JSON_TRANSFORM_COMPONENT_PIVOT_VAR "pivot"
#define JSON_TRANSFORM_COMPONENT_X_VAR "x"
#define JSON_TRANSFORM_COMPONENT_Y_VAR "y"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// TransformComponent
		//---------------------------------------------------------------------
		graphics::dx12::DX12Transform& TransformComponent::Transform()
		{
			return m_Transform;
		}

		//---------------------------------------------------------------------
#ifdef _EDITOR
		void TransformComponent::Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			a_Document.AddMember(JSON_TRANSFORM_COMPONENT_POSITION_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_TRANSFORM_COMPONENT_POSITION_VAR].AddMember(JSON_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetPosition().x, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_POSITION_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetPosition().y, a_Allocator);

			a_Document.AddMember(JSON_TRANSFORM_COMPONENT_ROTATION_VAR, m_Transform.GetRotation(), a_Allocator);

			a_Document.AddMember(JSON_TRANSFORM_COMPONENT_SCALE_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetScale().x, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetScale().y, a_Allocator);

			a_Document.AddMember(JSON_TRANSFORM_COMPONENT_PIVOT_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR].AddMember(JSON_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetPivot().x, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetPivot().y, a_Allocator);
		}
#endif

		//---------------------------------------------------------------------
		void TransformComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			m_Transform.SetPosition(a_SrcData.GetVector(JSON_TRANSFORM_COMPONENT_POSITION_VAR));
			m_Transform.SetScale(a_SrcData.GetVector(JSON_TRANSFORM_COMPONENT_SCALE_VAR));
			m_Transform.SetPivot(a_SrcData.GetVector(JSON_TRANSFORM_COMPONENT_PIVOT_VAR));
			m_Transform.SetRotation(a_SrcData.GetFloat(JSON_TRANSFORM_COMPONENT_ROTATION_VAR));
		}

		//---------------------------------------------------------------------
		void TransformComponent::Translate(const DirectX::XMFLOAT2& a_vTranslation)
		{
			m_vTranslation = {
				m_vTranslation.x + a_vTranslation.x,
				m_vTranslation.y + a_vTranslation.y
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
			DirectX::XMFLOAT2 newPos = {
				transformComp.Transform().GetPosition().x + m_vTranslation.x,
				transformComp.Transform().GetPosition().y + m_vTranslation.y,
			};
			transformComp.Transform().SetPosition(newPos);

			m_vTranslation = {};
		}
	}
}