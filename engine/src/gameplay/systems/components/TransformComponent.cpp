#include "gameplay/systems/components/TransformComponent.h"

#include <rapidjson/utils.h>
#include <DirectXMath.h>

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

		//---------------------------------------------------------------------
		void TransformComponent::Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator)
		{
			if (!a_Document.IsObject())
			{
				return;
			}

			if (a_Document.HasMember(JSON_TRANSFORM_COMPONENT_POSITION_VAR) && a_Document[JSON_TRANSFORM_COMPONENT_POSITION_VAR].IsObject())
			{
				DirectX::XMFLOAT2 position;
				rapidjson::GetFloat(a_Document[JSON_TRANSFORM_COMPONENT_POSITION_VAR], JSON_TRANSFORM_COMPONENT_X_VAR, position.x);
				rapidjson::GetFloat(a_Document[JSON_TRANSFORM_COMPONENT_POSITION_VAR], JSON_TRANSFORM_COMPONENT_Y_VAR, position.y);
				m_Transform.SetPosition(position);
			}

			if (a_Document.HasMember(JSON_TRANSFORM_COMPONENT_ROTATION_VAR) && a_Document[JSON_TRANSFORM_COMPONENT_ROTATION_VAR].IsFloat())
			{
				float rotation;
				rapidjson::GetFloat(a_Document, JSON_TRANSFORM_COMPONENT_ROTATION_VAR, rotation);
				m_Transform.SetRotation(rotation);
			}

			if (a_Document.HasMember(JSON_TRANSFORM_COMPONENT_SCALE_VAR) && a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR].IsObject())
			{
				DirectX::XMFLOAT2 scale;
				rapidjson::GetFloat(a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR], JSON_TRANSFORM_COMPONENT_X_VAR, scale.x);
				rapidjson::GetFloat(a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR], JSON_TRANSFORM_COMPONENT_Y_VAR, scale.y);
				m_Transform.SetScale(scale);
			}

			if (a_Document.HasMember(JSON_TRANSFORM_COMPONENT_PIVOT_VAR) && a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR].IsObject())
			{
				DirectX::XMFLOAT2 pivot;
				rapidjson::GetFloat(a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR], JSON_TRANSFORM_COMPONENT_X_VAR, pivot.x);
				rapidjson::GetFloat(a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR], JSON_TRANSFORM_COMPONENT_Y_VAR, pivot.y);
				m_Transform.SetPivot(pivot);
			}
		}
	}
}