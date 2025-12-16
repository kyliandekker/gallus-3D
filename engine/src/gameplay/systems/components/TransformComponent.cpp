#include "TransformComponent.h"

// external
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
#define JSON_TRANSFORM_COMPONENT_CAMERA_MODE_VAR "cameraMode"

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
		void TransformComponent::Serialize(resources::SrcData& a_SrcData) const
		{
			a_SrcData.SetVector3(JSON_TRANSFORM_COMPONENT_POSITION_VAR, m_Transform.GetPosition());
			a_SrcData.SetVector3(JSON_TRANSFORM_COMPONENT_SCALE_VAR, m_Transform.GetScale());
			DirectX::XMFLOAT3 rot = m_Transform.GetRotationV();
			a_SrcData.SetVector3(JSON_TRANSFORM_COMPONENT_ROTATION_VAR, rot);
			a_SrcData.SetVector3(JSON_TRANSFORM_COMPONENT_PIVOT_VAR, m_Transform.GetPivot());
			a_SrcData.SetEnum(JSON_TRANSFORM_COMPONENT_CAMERA_MODE_VAR, m_Transform.GetCameraType());
		}
#endif

		//---------------------------------------------------------------------
		void TransformComponent::Deserialize(const resources::SrcData& a_SrcData)
		{
			DirectX::XMFLOAT3 pos = VEC_ZERO;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_POSITION_VAR, pos))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_POSITION_VAR);
			}
			m_Transform.SetPosition(pos);

			DirectX::XMFLOAT3 scale = VEC_IDENTITY;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_SCALE_VAR, scale))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_SCALE_VAR);
			}
			m_Transform.SetScale(scale);

			DirectX::XMFLOAT3 pivot = VEC_PIVOT_CENTER;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_PIVOT_VAR, pivot))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_PIVOT_VAR);
			}
			m_Transform.SetPivot(pivot);

			DirectX::XMFLOAT3 rotation = VEC_ZERO;
			if (!a_SrcData.GetVector3(JSON_TRANSFORM_COMPONENT_ROTATION_VAR, rotation))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Transform component did not have key %s present in its meta data.", JSON_TRANSFORM_COMPONENT_ROTATION_VAR);
			}
			m_Transform.SetRotation(graphics::dx12::DX12Transform::EulerToQuaternion(rotation));

			graphics::dx12::CameraType cameraType = graphics::dx12::CameraType::CameraType_World;
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