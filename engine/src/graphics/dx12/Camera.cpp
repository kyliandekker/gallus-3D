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
				SetProjection(a_fWidth, a_fHeight, -1.0f, 1.0f);
			}

			//---------------------------------------------------------------------
			void Camera::SetProjection(float a_fWidth, float a_fHeight, float a_fNearPlane, float a_fFarPlane)
			{
				m_ProjectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
					0.0f, a_fWidth,
					a_fHeight, 0.0f,
					a_fNearPlane, a_fFarPlane
				);
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
				const DirectX::XMFLOAT2& pos = m_Transform.GetPosition();
				const float rotZ = DirectX::XMConvertToRadians(m_Transform.GetRotation());
				const DirectX::XMFLOAT2& scale = m_Transform.GetScale();

				const DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(pos.x, pos.y, 0.0f);
				const DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(rotZ);
				const DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(scale.x, scale.y, 1.0f);

				const DirectX::XMMATRIX world = scaling * rotation * translation;

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