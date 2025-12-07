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
				m_fRotationDegrees(0)
			{}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetPosition(const DirectX::XMFLOAT3& a_vPosition)
			{
				m_vPosition = a_vPosition;
			}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetRotation(float a_fRotationDegrees)
			{
				m_fRotationDegrees = a_fRotationDegrees;
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
			float DX12Transform2D::GetRotation() const
			{
				return m_fRotationDegrees;
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
				DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
				float radians = DirectX::XMConvertToRadians(m_fRotationDegrees);
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(radians);
				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);

				return scaleMatrix * rotationMatrix * translationMatrix;
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX DX12Transform2D::GetWorldMatrixWithPivot() const
			{
				DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_vScale.x, m_vScale.y, m_vScale.z);
				float radians = DirectX::XMConvertToRadians(m_fRotationDegrees);
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(radians);
				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_vPosition.x, m_vPosition.y, m_vPosition.z);

				DirectX::XMMATRIX pivotTranslation = DirectX::XMMatrixTranslation(m_vPivot.x, m_vPivot.y, m_vPivot.z);
				DirectX::XMMATRIX invPivotTranslation = DirectX::XMMatrixTranslation(-m_vPivot.x, -m_vPivot.y, -m_vPivot.z);

				// World = Translation * Pivot * Rotation * Scale * InversePivot
				return invPivotTranslation * scaleMatrix * rotationMatrix * pivotTranslation * translationMatrix;
			}

			//---------------------------------------------------------------------
			void DX12Transform2D::SetWorldMatrix(const DirectX::XMMATRIX& a_WorldMatrix)
			{
				m_vPosition.x = DirectX::XMVectorGetX(a_WorldMatrix.r[3]);
				m_vPosition.y = DirectX::XMVectorGetY(a_WorldMatrix.r[3]);
				m_vPosition.z = DirectX::XMVectorGetZ(a_WorldMatrix.r[3]);

				DirectX::XMVECTOR column0 = a_WorldMatrix.r[0]; // X axis
				DirectX::XMVECTOR column1 = a_WorldMatrix.r[1]; // Y axis
				DirectX::XMVECTOR column2 = a_WorldMatrix.r[2]; // Z axis

				m_vScale.x = DirectX::XMVectorGetX(DirectX::XMVector3Length(column0));
				m_vScale.y = DirectX::XMVectorGetX(DirectX::XMVector3Length(column1));
				m_vScale.z = DirectX::XMVectorGetX(DirectX::XMVector3Length(column2));

				DirectX::XMVECTOR normX = DirectX::XMVectorScale(column0, 1.0f / m_vScale.x);
				DirectX::XMVECTOR normY = DirectX::XMVectorScale(column1, 1.0f / m_vScale.y);

				float radians = std::atan2(
					DirectX::XMVectorGetY(normX),
					DirectX::XMVectorGetX(normX)
				);

				m_fRotationDegrees = DirectX::XMConvertToDegrees(radians);

				if (m_fRotationDegrees < 0.0f)
				{
					m_fRotationDegrees += 360.0f;
				}
			}

		}
	}
}