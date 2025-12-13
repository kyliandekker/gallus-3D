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
#ifdef _EDITOR
				: IExposableToEditor()
#endif
			{}

			//---------------------------------------------------------------------
			void Camera::Init(float a_fWidth, float a_fHeight)
			{
				m_vSize = { static_cast<int>(a_fWidth), static_cast<int>(a_fHeight) };
				//SetProjection(a_fWidth, a_fHeight, -1.0f, 1.0f);
				SetProjection(a_fWidth, a_fHeight, 0.1f, 1000.0f);
			}

			//---------------------------------------------------------------------
			void Camera::SetProjection(float a_fWidth, float a_fHeight, float a_fNearPlane, float a_fFarPlane)
			{
				//m_ProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
				//	0.0f, a_fWidth,
				//	a_fHeight, 0.0f,
				//	a_fNearPlane, a_fFarPlane
				//);
				m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(
					DirectX::XMConvertToRadians(m_fFov), a_fWidth / a_fHeight, a_fNearPlane, a_fFarPlane
				);
			}

			//---------------------------------------------------------------------
			DX12Transform2D& Camera::Transform()
			{
				return m_Transform;
			}

			//---------------------------------------------------------------------
			const DX12Transform2D& Camera::Transform() const
			{
				return m_Transform;
			}

			//---------------------------------------------------------------------
			DirectX::XMMATRIX Camera::GetViewMatrix() const
			{
				const DirectX::XMFLOAT3& pos = m_Transform.GetPosition();

				const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
				DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(m_Transform.GetRotationQ());

				const DirectX::XMMATRIX world = rotation * translation;

				return DirectX::XMMatrixInverse(nullptr, world);
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
			{
				return m_ProjectionMatrix;
			}
		}
	}
}