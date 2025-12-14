#include "TransformComponent.h"

// external
#include <rapidjson/utils.h>
#include <DirectXMath.h>

// resources
#include "resources/SrcData.h"

// gameplay
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/EntityID.h"

#include "logger/Logger.h"

#define JSON_TRANSFORM_COMPONENT_POSITION_VAR "position"
#define JSON_TRANSFORM_COMPONENT_ROTATION_VAR "rotation"
#define JSON_TRANSFORM_COMPONENT_SCALE_VAR "scale"
#define JSON_TRANSFORM_COMPONENT_PIVOT_VAR "pivot"
#define JSON_TRANSFORM_COMPONENT_X_VAR "x"
#define JSON_TRANSFORM_COMPONENT_Y_VAR "y"
#define JSON_TRANSFORM_COMPONENT_Z_VAR "z"
#define JSON_TRANSFORM_COMPONENT_CAMERA_MODE_VAR "cameraMode"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// TransformComponent
		//---------------------------------------------------------------------
		graphics::dx12::DX12Transform2D& TransformComponent::Transform()
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
			a_Document[JSON_TRANSFORM_COMPONENT_POSITION_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetPosition().z, a_Allocator);
			
			a_Document.AddMember(JSON_TRANSFORM_COMPONENT_ROTATION_VAR, rapidjson::Value().SetObject(), a_Allocator);

			DirectX::XMFLOAT3 rot = m_Transform.GetRotationV();
			a_Document[JSON_TRANSFORM_COMPONENT_ROTATION_VAR].AddMember(JSON_TRANSFORM_COMPONENT_X_VAR, rot.x, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_ROTATION_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Y_VAR, rot.y, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_ROTATION_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Z_VAR, rot.z, a_Allocator);

			a_Document.AddMember(JSON_TRANSFORM_COMPONENT_SCALE_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetScale().x, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetScale().y, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_SCALE_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetScale().z, a_Allocator);

			a_Document.AddMember(JSON_TRANSFORM_COMPONENT_PIVOT_VAR, rapidjson::Value().SetObject(), a_Allocator);

			a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR].AddMember(JSON_TRANSFORM_COMPONENT_X_VAR, m_Transform.GetPivot().x, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Y_VAR, m_Transform.GetPivot().y, a_Allocator);
			a_Document[JSON_TRANSFORM_COMPONENT_PIVOT_VAR].AddMember(JSON_TRANSFORM_COMPONENT_Z_VAR, m_Transform.GetPivot().z, a_Allocator);

			a_Document.AddMember(
				JSON_TRANSFORM_COMPONENT_CAMERA_MODE_VAR,
				(int)m_Transform.GetCameraType(),
				a_Allocator
			);
		}
#endif

		//---------------------------------------------------------------------
		void TransformComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			DirectX::XMFLOAT3 pos;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_POSITION_VAR, pos))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_POSITION_VAR);
			}
			m_Transform.SetPosition(pos);

			DirectX::XMFLOAT3 scale;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_SCALE_VAR, scale))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_SCALE_VAR);
			}
			m_Transform.SetScale(scale);

			DirectX::XMFLOAT3 pivot;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_PIVOT_VAR, pivot))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_PIVOT_VAR);
			}
			m_Transform.SetPivot(pivot);

			DirectX::XMFLOAT3 rotation;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_ROTATION_VAR, rotation))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_ROTATION_VAR);
			}
			m_Transform.SetRotation(graphics::dx12::DX12Transform2D::EulerToQuaternion(rotation));

			graphics::dx12::CameraType cameraType;
			if (!a_SrcData.GetEnum(JSON_TRANSFORM_COMPONENT_CAMERA_MODE_VAR, cameraType))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_CAMERA_MODE_VAR);
			}
			m_Transform.SetCameraMode(cameraType);
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
				transformComp.Transform().GetPosition().x + m_vTranslation.x,
				transformComp.Transform().GetPosition().y + m_vTranslation.y,
				transformComp.Transform().GetPosition().z + m_vTranslation.z
			};
			transformComp.Transform().SetPosition(newPos);

			m_vTranslation = {};
		}
	}
}