#include "DX12Transform2D.h"

// external
#include <iostream>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// DX12Transform2D
			//---------------------------------------------------------------------
			DX12Transform2D::DX12Transform2D() : 
				m_vPosition{ 0.0f, 0.0f, 0.0f },
				m_vScale{ 1.0f, 1.0f, 1.0f },
				m_vPivot{ -0.5f, -0.5f, -0.5f },
				m_vRotation(0, 0, 0)
			{}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetPosition(const DirectX::XMFLOAT3& a_vPosition)
			{
				m_vPosition = a_vPosition;
			}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetRotation(const DirectX::XMVECTOR& a_vRotation)
			{
				m_vRotation = a_vRotation;
			}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetScale(const DirectX::XMFLOAT3& a_Scale)
			{
				m_vScale = a_Scale;
			}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetPivot(const DirectX::XMFLOAT3& a_vPivot)
			{
				m_vPivot = a_vPivot;
				if (m_vPivot.x < -0.5f)
				{
					m_vPivot.x = -0.5f;
				}
				else if (m_vPivot.x > 0.5f)
				{
					m_vPivot.x = 0.5f;
				}
				if (m_vPivot.y < -0.5f)
				{
					m_vPivot.y = -0.5f;
				}
				else if (m_vPivot.y > 0.5f)
				{
					m_vPivot.y = 0.5f;
				}
				if (m_vPivot.z < -0.5f)
				{
					m_vPivot.z = -0.5f;
				}
				else if (m_vPivot.z > 0.5f)
				{
					m_vPivot.z = 0.5f;
				}
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT3& DX12Transform2D::GetPosition() const
			{
				return m_vPosition;
			}

			//---------------------------------------------------------------------
			DirectX::XMFLOAT3 DX12Transform2D::QuaternionToEuler(const DirectX::XMVECTOR& quat)
			{
				// Normalize the quaternion to avoid errors due to floating-point imprecision
				DirectX::XMVECTOR q = quat;

				// Extract the components of the normalized quaternion
				float x = DirectX::XMVectorGetX(q);
				float y = DirectX::XMVectorGetY(q);
				float z = DirectX::XMVectorGetZ(q);
				float w = DirectX::XMVectorGetW(q);

				// Compute Euler angles
				DirectX::XMFLOAT3 euler;

				// Roll (X-axis rotation)
				float sinr_cosp = 2.0f * (w * x + y * z);
				float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
				euler.x = std::atan2(sinr_cosp, cosr_cosp);

				// Pitch (Y-axis rotation)
				float sinp = 2.0f * (w * y - z * x);
				if (std::abs(sinp) >= 1)
					euler.y = std::copysign(DirectX::XM_PI / 2, sinp); // Use 90 degrees if out of range
				else
					euler.y = std::asin(sinp);

				// Yaw (Z-axis rotation)
				float siny_cosp = 2.0f * (w * z + x * y);
				float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
				euler.z = std::atan2(siny_cosp, cosy_cosp);

				// Convert radians to degrees
				euler.x = DirectX::XMConvertToDegrees(euler.x);
				euler.y = DirectX::XMConvertToDegrees(euler.y);
				euler.z = DirectX::XMConvertToDegrees(euler.z);

				return euler;
			}
			
			//---------------------------------------------------------------------
			DirectX::XMVECTOR DX12Transform2D::AddRotation(DirectX::XMVECTOR& a_vQuat, const DirectX::XMFLOAT3& a_vAddition)
			{
				float pitch = DirectX::XMConvertToRadians(a_vAddition.x);
				float yaw = DirectX::XMConvertToRadians(a_vAddition.y);
				float roll = DirectX::XMConvertToRadians(a_vAddition.z);

				// Build quaternion directly from Euler angles
				DirectX::XMVECTOR deltaQuat = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

				// Accumulate: local rotation (delta * current) or global (current * delta)
				a_vQuat = DirectX::XMQuaternionMultiply(deltaQuat, a_vQuat);

				// Normalize to avoid drift
				a_vQuat = DirectX::XMQuaternionNormalize(a_vQuat);

				return a_vQuat;
			}

			//---------------------------------------------------------------------
			const DirectX::XMVECTOR& DX12Transform2D::GetRotationQ() const
			{
				return m_vRotation;
			}
			
			//---------------------------------------------------------------------
			DirectX::XMFLOAT3 DX12Transform2D::GetRotationV() const
			{
				return QuaternionToEuler(m_vRotation);
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT3& DX12Transform2D::GetScale() const
			{
				return m_vScale;
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT3& DX12Transform2D::GetPivot() const
			{
				return m_vPivot;
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX DX12Transform2D::GetWorldMatrix() const
			{
				// Use quaternion for rotation
				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_vPosition));
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vRotation);
				DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_vScale));

				return scaleMatrix * rotationMatrix * translationMatrix;
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX DX12Transform2D::GetWorldMatrixWithPivot() const
			{
				// Use quaternion for rotation
				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_vPosition));
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vRotation);
				DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_vScale));

				DirectX::XMMATRIX pivotTranslation = DirectX::XMMatrixTranslation(m_vPivot.x, m_vPivot.y, m_vPivot.z);
				DirectX::XMMATRIX invPivotTranslation = DirectX::XMMatrixTranslation(-m_vPivot.x, -m_vPivot.y, -m_vPivot.z);

				return invPivotTranslation * scaleMatrix * rotationMatrix * pivotTranslation * translationMatrix;
			}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetWorldMatrix(const DirectX::XMMATRIX& a_WorldMatrix)
			{
				DirectX::XMVECTOR scale, rotation, translation;
				if (DirectX::XMMatrixDecompose(&scale, &rotation, &translation, a_WorldMatrix))
				{
					m_vPosition = DirectX::XMFLOAT3(DirectX::XMVectorGetX(translation),
						DirectX::XMVectorGetY(translation),
						DirectX::XMVectorGetZ(translation));

					m_vScale = DirectX::XMFLOAT3(DirectX::XMVectorGetX(scale),
						DirectX::XMVectorGetY(scale),
						DirectX::XMVectorGetZ(scale));

					m_vRotation = rotation;
				}
			}

		}
	}
}