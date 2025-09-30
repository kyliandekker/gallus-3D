#include "Camera.h"

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
			{}

			//---------------------------------------------------------------------
			void Camera::Init(float a_fWidth, float a_fHeight)
			{
				m_vSize = { static_cast<int>(a_fWidth), static_cast<int>(a_fHeight) };
				SetProjectionMode(CameraProjectionMode::Perspective);
			}

			//---------------------------------------------------------------------
			void Camera::SetProjection(float a_fWidth, float a_fHeight, float a_fNearPlane, float a_fFarPlane)
			{
				if (m_CameraProjectionMode == CameraProjectionMode::Orthographic)
				{
					m_ProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
						0.0f, a_fWidth,
						a_fHeight, 0.0f,
						0, a_fFarPlane
					);
				}
				else
				{
					m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
						DirectX::XMConvertToRadians(m_fFoV), a_fWidth / a_fHeight, a_fNearPlane, a_fFarPlane
					);
				}
			}

			//---------------------------------------------------------------------
			DX12Transform& Camera::Transform()
			{
				return m_Transform;
			}

			//---------------------------------------------------------------------
			const DX12Transform& Camera::Transform() const
			{
				return m_Transform;
			}

			//---------------------------------------------------------------------
			DirectX::XMMATRIX Camera::GetViewMatrix() const
			{
				const DirectX::XMFLOAT3& pos = m_Transform.GetPosition();

				const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(m_Transform.GetRotationQuaternion());

				const DirectX::XMMATRIX world = rotation * translation;

				return DirectX::XMMatrixInverse(nullptr, world);
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
			{
				return m_ProjectionMatrix;
			}

			//---------------------------------------------------------------------
			void Camera::SetProjectionMode(CameraProjectionMode a_CameraProjectionMode)
			{
				if (m_CameraProjectionMode == a_CameraProjectionMode)
				{
					return;
				}

				m_CameraProjectionMode = a_CameraProjectionMode;
				SetProjection(m_vSize.x, m_vSize.y, 0.1f, 1000.0f);
			}

			//---------------------------------------------------------------------
			CameraProjectionMode Camera::GetCameraProjectionMode() const
			{
				return m_CameraProjectionMode;
			}
		}
	}
}