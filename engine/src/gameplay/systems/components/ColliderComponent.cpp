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
	}
}