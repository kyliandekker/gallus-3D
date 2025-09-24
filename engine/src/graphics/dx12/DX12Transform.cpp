#include "DX12Transform.h"

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
			DX12Transform::DX12Transform()
				: m_vPosition{ 0.0f, 0.0f },
				m_fRotationDegrees(0),
				m_vScale{ 1.0f, 1.0f }
			{}

			//---------------------------------------------------------------------
			void DX12Transform::SetPosition(const DirectX::XMFLOAT2& a_vPosition)
			{
				m_vPosition = a_vPosition;
			}

			//---------------------------------------------------------------------
			void DX12Transform::SetRotation(float a_fRotationDegrees)
			{
				m_fRotationDegrees = a_fRotationDegrees;
			}

			//---------------------------------------------------------------------
			void DX12Transform::SetScale(const DirectX::XMFLOAT2& a_Scale)
			{
				m_vScale = a_Scale;
			}

			//---------------------------------------------------------------------
			void DX12Transform::SetPivot(const DirectX::XMFLOAT2& a_vPivot)
			{
				m_vPivot = a_vPivot;
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT2& DX12Transform::GetPosition() const
			{
				return m_vPosition;
			}

			//---------------------------------------------------------------------
			float DX12Transform::GetRotation() const
			{
				return m_fRotationDegrees;
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT2& DX12Transform::GetScale() const
			{
				return m_vScale;
			}

			//---------------------------------------------------------------------
			const DirectX::XMFLOAT2& DX12Transform::GetPivot() const
			{
				return m_vPivot;
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX DX12Transform::GetWorldMatrix() const
			{
				DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_vScale.x, m_vScale.y, 1.0f);
				float radians = DirectX::XMConvertToRadians(m_fRotationDegrees);
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(radians);
				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_vPosition.x, m_vPosition.y, 0.0f);

				return scaleMatrix * rotationMatrix * translationMatrix;
			}

			//---------------------------------------------------------------------
			const DirectX::XMMATRIX DX12Transform::GetWorldMatrixWithPivot() const
			{
				DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(m_vScale.x, m_vScale.y, 1.0f);
				float radians = DirectX::XMConvertToRadians(m_fRotationDegrees);
				DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationZ(radians);
				DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_vPosition.x, m_vPosition.y, 0.0f);

				DirectX::XMMATRIX pivotTranslation = DirectX::XMMatrixTranslation(m_vPivot.x, m_vPivot.y, 0.0f);
				DirectX::XMMATRIX invPivotTranslation = DirectX::XMMatrixTranslation(-m_vPivot.x, -m_vPivot.y, 0.0f);

				// World = Translation * Pivot * Rotation * Scale * InversePivot
				return invPivotTranslation * scaleMatrix * rotationMatrix * pivotTranslation * translationMatrix;
			}

			//---------------------------------------------------------------------
			void DX12Transform::SetWorldMatrix(const DirectX::XMMATRIX& a_WorldMatrix)
			{
				m_vPosition.x = DirectX::XMVectorGetX(a_WorldMatrix.r[3]);
				m_vPosition.y = DirectX::XMVectorGetY(a_WorldMatrix.r[3]);

				DirectX::XMVECTOR column0 = a_WorldMatrix.r[0];
				DirectX::XMVECTOR column1 = a_WorldMatrix.r[1];

				m_vScale.x = DirectX::XMVectorGetX(DirectX::XMVector2Length(column0));
				m_vScale.y = DirectX::XMVectorGetX(DirectX::XMVector2Length(column1));

				DirectX::XMVECTOR normX = DirectX::XMVectorScale(column0, 1.0f / m_vScale.x);
				DirectX::XMVECTOR normY = DirectX::XMVectorScale(column1, 1.0f / m_vScale.y);

				float radians = std::atan2(DirectX::XMVectorGetY(normX), DirectX::XMVectorGetX(normX));
				m_fRotationDegrees = DirectX::XMConvertToDegrees(radians);

				if (m_fRotationDegrees < 0.0f)
				{
					m_fRotationDegrees += 360.0f;
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

				DirectX::XMFLOAT2 pivotShift(-m_vPivot.x, -m_vPivot.y);

				float rad = DirectX::XMConvertToRadians(m_fRotationDegrees);
				float cosR = cosf(rad);
				float sinR = sinf(rad);

				for (auto& c : corners)
				{
					c.x += pivotShift.x;
					c.y += pivotShift.y;

					c.x *= m_vScale.x;
					c.y *= m_vScale.y;

					float rx = c.x * cosR - c.y * sinR;
					float ry = c.x * sinR + c.y * cosR;
					c.x = rx + m_vPosition.x;
					c.y = ry + m_vPosition.y;
				}

				return corners;
			}
		}
	}
}