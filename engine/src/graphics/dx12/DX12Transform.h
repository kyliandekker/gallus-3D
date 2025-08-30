#pragma once

#include <DirectXMath.h>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// DX12Transform
			//---------------------------------------------------------------------
			class DX12Transform
			{
			public:
				DX12Transform();

				/// <summary>
				/// Sets the position of the transform.
				/// </summary>
				/// <param name="a_Position">A XMFLOAT2 containing the position.</param>
				void SetPosition(const DirectX::XMFLOAT2& a_vPosition);

				/// <summary>
				/// Sets the rotation of the transform.
				/// </summary>
				/// <param name="a_fRotationDegrees">A float containing the rotation in degrees.</param>
				void SetRotation(float a_fRotationDegrees); // Rotation in degrees

				/// <summary>
				/// Sets the scale of the transform.
				/// </summary>
				/// <param name="a_Position">A XMFLOAT2 containing the scale.</param>
				void SetScale(const DirectX::XMFLOAT2& a_vScale);

				/// <summary>
				/// Retrieves the position of the transform.
				/// </summary>
				/// <returns>A XMFLOAT2 containing the position.</returns>
				const DirectX::XMFLOAT2& GetPosition() const;

				/// <summary>
				/// Retrieves the rotation of the transform.
				/// </summary>
				/// <returns>A float containing the rotation in degrees.</returns>
				float GetRotation() const;

				/// <summary>
				/// Retrieves the scale of the transform.
				/// </summary>
				/// <returns>A XMFLOAT2 containing the scale.</returns>
				const DirectX::XMFLOAT2& GetScale() const;

				const DirectX::XMMATRIX GetScaleMatrix() const;
				const DirectX::XMMATRIX GetRotationMatrix() const;
				const DirectX::XMMATRIX GetPositionMatrix() const;
				const DirectX::XMMATRIX GetMatrix() const;

				/// <summary>
				/// Sets the world matrix.
				/// </summary>
				/// <param name="worldMatrix">A XMMATRIX containing the world matrix for the transform.</param>
				void SetWorldMatrix(const DirectX::XMMATRIX& a_WorldMatrix);
			private:
				DirectX::XMFLOAT2 m_vPosition = { 0, 0 };
				float m_fRotationDegrees = 0.0f; // rotation around Z axis
				DirectX::XMFLOAT2 m_vScale = { 1, 1 };
			};
		}
	}
}