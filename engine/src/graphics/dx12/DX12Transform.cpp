// header
#include "DX12Transform.h"

// external
#include <iostream>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// DX12Transform
			//---------------------------------------------------------------------
			DX12Transform::DX12Transform() : 
				m_vPosition{ 0.0f, 0.0f, 0.0f },
				m_vRotation{ DirectX::XMQuaternionIdentity() },
				m_vScale{ 1.0f, 1.0f, 1.0f }
			{}

			//---------------------------------------------------------------------
			void DX12Transform::SetPosition(const DirectX::XMFLOAT3& a_vPosition)
			{
				m_vPosition = a_vPosition;
			}

			//---------------------------------------------------------------------
			void DX12Transform::AddRotation(const DirectX::XMFLOAT3& a_Rotation)
			{
				float pitch = DirectX::XMConvertToRadians(a_Rotation.x);
				float yaw = DirectX::XMConvertToRadians(a_Rotation.y);
				float roll = DirectX::XMConvertToRadians(a_Rotation.z);

				// Build quaternion directly from Euler angles
				DirectX::XMVECTOR deltaQuat = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

				// Accumulate: local rotation (delta * current) or global (current * delta)
				m_vRotation = DirectX::XMQuaternionMultiply(deltaQuat, m_vRotation);

				// Normalize to avoid drift
				m_vRotation = DirectX::XMQuaternionNormalize(m_vRotation);
			}

			//---------------------------------------------------------------------
			void DX12Transform::SetRotation(const DirectX::XMFLOAT3& a_Rotation)
			{
				float pitch = DirectX::XMConvertToRadians(a_Rotation.x);
				float yaw = DirectX::XMConvertToRadians(a_Rotation.y);
				float roll = DirectX::XMConvertToRadians(a_Rotation.z);

				// Create quaternions for each rotation component
				DirectX::XMVECTOR pitchQuat = DirectX::XMQuaternionRotationAxis(DirectX::g_XMIdentityR0, pitch);
				DirectX::XMVECTOR yawQuat = DirectX::XMQuaternionRotationAxis(DirectX::g_XMIdentityR1, yaw);
				DirectX::XMVECTOR rollQuat = DirectX::XMQuaternionRotationAxis(DirectX::g_XMIdentityR2, roll);

				// Combine the rotations (roll * pitch * yaw)
				DirectX::XMVECTOR newRotation = DirectX::XMQuaternionMultiply(pitchQuat, DirectX::XMQuaternionMultiply(yawQuat, rollQuat));

				// Normalize the resulting quaternion
				m_vRotation = DirectX::XMQuaternionNormalize(newRotation);
			}

			//---------------------------------------------------------------------
			void DX12Transform::SetScale(const DirectX::XMFLOAT3& a_Scale)
			{
				m_vScale = a_Scale;
			}

			//---------------------------------------------------------------------
			void DX12Transform::SetPivot(const DirectX::XMFLOAT3& a_vPivot)
			{
				m_vPivot = a_vPivot;
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT3& DX12Transform::GetPosition() const
			{
				return m_vPosition;
			}

			//---------------------------------------------------------------------
			DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMFLOAT4& quat)
			{
				// Load the quaternion into an XMVECTOR
				DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&quat);

				// Normalize the quaternion to avoid errors due to floating-point imprecision
				q = DirectX::XMQuaternionNormalize(q);

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
			DirectX::XMFLOAT3 DX12Transform::GetRotation() const
			{
				// Convert quaternion back to Euler angles
				DirectX::XMFLOAT3 euler;
				DirectX::XMFLOAT4 quat;

				// Convert quaternion to Euler angles
				DirectX::XMStoreFloat4(&quat, m_vRotation);
				euler = QuaternionToEuler(quat); // Convert quaternion to Euler angles

				return euler;
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT3& DX12Transform::GetScale() const
			{
				return m_vScale;
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT3& DX12Transform::GetPivot() const
			{
				return m_vPivot;
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX DX12Transform::GetWorldMatrix() const
			{
				// Use quaternion for rotation
				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_vPosition));
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(m_vRotation);
				DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_vScale));

				return scaleMatrix * rotationMatrix * translationMatrix;
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX DX12Transform::GetWorldMatrixWithPivot() const
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
			void DX12Transform::SetWorldMatrix(const DirectX::XMMATRIX& a_WorldMatrix)
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

			//---------------------------------------------------------------------
			std::array<DirectX::XMFLOAT2, 4> DX12Transform::GetWorldCorners() const
			{
				std::array<DirectX::XMFLOAT2, 4> corners = {
					DirectX::XMFLOAT2(-0.5f, -0.5f),
					DirectX::XMFLOAT2(0.5f, -0.5f),
					DirectX::XMFLOAT2(0.5f, 0.5f),
					DirectX::XMFLOAT2(-0.5f, 0.5f)
				};

				//DirectX::XMFLOAT2 pivotShift(-m_vPivot.x, -m_vPivot.y);

				//float rad = DirectX::XMConvertToRadians(m_fRotationDegrees);
				//float cosR = cosf(rad);
				//float sinR = sinf(rad);

				//for (auto& c : corners)
				//{
				//	c.x += pivotShift.x;
				//	c.y += pivotShift.y;

				//	c.x *= m_vScale.x;
				//	c.y *= m_vScale.y;

				//	float rx = c.x * cosR - c.y * sinR;
				//	float ry = c.x * sinR + c.y * cosR;
				//	c.x = rx + m_vPosition.x;
				//	c.y = ry + m_vPosition.y;
				//}

				return corners;
			}
		}
	}
}