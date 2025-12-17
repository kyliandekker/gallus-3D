#include "Camera.h"

#include "utils/general_utils.h"

#include "editor/EditorExpose.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Camera
			//---------------------------------------------------------------------
			Camera::Camera()
#ifdef _EDITOR
				: IExposableToEditor()
#endif // _EDITOR
			{ }
			
			//---------------------------------------------------------------------
			void Camera::Init(float a_fWidth, float a_fHeight)
			{
				m_fFoV = 90;
				m_Transform = Transform();

				m_vSize = { static_cast<int>(a_fWidth), static_cast<int>(a_fHeight) };
				SetProjection();
			}
			
			//---------------------------------------------------------------------
			void Camera::Deserialize(const resources::SrcData& a_SrcData)
			{
				DeserializeEditorExposable(this, a_SrcData);
			}

			//---------------------------------------------------------------------
			void Camera::SetProjection()
			{
				m_ProjectionMatrix2D = DirectX::XMMatrixOrthographicOffCenterLH(
					0.0f, static_cast<float>(m_vSize.x),
					static_cast<float>(m_vSize.y), 0.0f,
					-1.0f, 1.0f
				);
				m_ProjectionMatrix3D = DirectX::XMMatrixPerspectiveFovLH(
					DirectX::XMConvertToRadians(m_fFoV),
					static_cast<float>(m_vSize.x) / static_cast<float>(m_vSize.y),
					0.1f,
					1000.0f
				);
			}

			//---------------------------------------------------------------------
			Transform& Camera::GetTransform()
			{
				return m_Transform;
			}

			//---------------------------------------------------------------------
			const Transform& Camera::GetTransform() const
			{
				return m_Transform;
			}

			//---------------------------------------------------------------------
			DirectX::XMMATRIX Camera::GetViewMatrix(CameraType a_CameraType) const
			{
				if (a_CameraType == CameraType::CameraType_World)
				{
					const DirectX::XMFLOAT3& pos = m_Transform.GetPosition();

					const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
					DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(m_Transform.GetRotationQ());

					const DirectX::XMMATRIX world = rotation * translation;

					return DirectX::XMMatrixInverse(nullptr, world);
				}
				else
				{
					const DirectX::XMFLOAT3& pos = m_Transform.GetPosition();
					const float rotZ = DirectX::XMConvertToRadians(m_Transform.GetRotationV().y);
					const DirectX::XMFLOAT3& scale = {
						m_Transform.GetScale().x,
						m_Transform.GetScale().y,
						0
					};

					const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(pos.x, pos.y, 0.0f);
					const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(rotZ);

					const DirectX::XMMATRIX world = rotation * translation;

					return DirectX::XMMatrixInverse(nullptr, world);
				}
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX& Camera::GetProjectionMatrix(CameraType a_CameraType) const
			{
				if (a_CameraType == CameraType::CameraType_World)
				{
					return m_ProjectionMatrix3D;
				}
				else
				{
					return m_ProjectionMatrix2D;
				}
			}
			
			//---------------------------------------------------------------------
			void Camera::OnFoVChanged()
			{
				// Clamp FoV.
				m_fFoV = utils::clamp(m_fFoV, 30.0f, 120.0f);

				SetProjection();
			}
		}
	}
}