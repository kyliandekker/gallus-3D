#pragma once

// external
#include <DirectXMath.h>
#include <array>

constexpr DirectX::XMVECTOR UP = { 0, 1, 0 }; /// Up vector.
constexpr DirectX::XMVECTOR FORWARD = { 0, 0, 1 }; /// Forward vector.
inline const DirectX::XMMATRIX IDENTITY = { /// Identity matrix.
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
};

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
				/// <param name="a_vPosition">A XMFLOAT3 containing the position.</param>
				void SetPosition(const DirectX::XMFLOAT3& a_vPosition);

				void AddRotation(const DirectX::XMFLOAT3& a_Rotation);

				/// <summary>
				/// Sets the rotation of the transform.
				/// </summary>
				void SetRotation(const DirectX::XMFLOAT3& a_Rotation);

				/// <summary>
				/// Sets the scale of the transform.
				/// </summary>
				/// <param name="a_vScale">A XMFLOAT3 containing the scale.</param>
				void SetScale(const DirectX::XMFLOAT3& a_vScale);

				/// <summary>
				/// Sets the pivot of the transform.
				/// </summary>
				/// <param name="a_vPivot">A XMFLOAT3 containing the pivot.</param>
				void SetPivot(const DirectX::XMFLOAT3& a_vPivot);

				/// <summary>
				/// Retrieves the position of the transform.
				/// </summary>
				/// <returns>A XMFLOAT2 containing the position.</returns>
				const DirectX::XMFLOAT3& GetPosition() const;

				/// <summary>
				/// Retrieves the rotation of the transform.
				/// </summary>
				DirectX::XMFLOAT3 GetRotation() const;

				/// <summary>
				/// Retrieves the rotation of the transform.
				/// </summary>
				const DirectX::XMVECTOR& GetRotationQuaternion() const
				{
					return m_vRotation;
				}

				/// <summary>
				/// Retrieves the scale of the transform.
				/// </summary>
				/// <returns>A XMFLOAT2 containing the scale.</returns>
				const DirectX::XMFLOAT3& GetScale() const;

				/// <summary>
				/// Retrieves the pivot of the transform.
				/// </summary>
				/// <returns>A XMFLOAT2 containing the pivot.</returns>
				const DirectX::XMFLOAT3& GetPivot() const;

				/// <summary>
				/// Retrieves the world matrix.
				/// </summary>
				/// <returns>A XMMATRIX containing the world matrix for the transform.</returns>
				const DirectX::XMMATRIX GetWorldMatrix() const;

				/// <summary>
				/// Retrieves the world matrix.
				/// </summary>
				/// <returns>A XMMATRIX containing the world matrix for the transform.</returns>
				const DirectX::XMMATRIX GetWorldMatrixWithPivot() const;

				/// <summary>
				/// Sets the world matrix.
				/// </summary>
				/// <param name="worldMatrix">A XMMATRIX containing the world matrix for the transform.</param>
				void SetWorldMatrix(const DirectX::XMMATRIX& a_WorldMatrix);

				/// <summary>
				/// Retrieves the world corners for collision purposes.
				/// </summary>
				/// <returns>An array containing 4 corners in screen space.</returns>
				std::array<DirectX::XMFLOAT2, 4> GetWorldCorners() const;
			private:
				DirectX::XMFLOAT3 m_vPosition = { 0, 0, 0 };
				DirectX::XMVECTOR m_vRotation = { DirectX::XMQuaternionIdentity() };
				DirectX::XMFLOAT3 m_vScale = { 1, 1, 1 };
				DirectX::XMFLOAT3 m_vPivot = { 0.0f, 0.0f, 0.0f };
			};
		}
	}
}