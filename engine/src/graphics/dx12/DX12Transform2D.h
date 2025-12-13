#pragma once

// external
#include <DirectXMath.h>
#include <array>

#ifdef _EDITOR
#include "editor/EditorExpose.h"
#endif

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
			// DX12Transform2D
			//---------------------------------------------------------------------
			class DX12Transform2D
#ifdef _EDITOR
				: public IExposableToEditor
#endif
			{
			public:
				static DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMVECTOR& quat);
				static DirectX::XMVECTOR AddRotation(DirectX::XMVECTOR& a_vQuat, const DirectX::XMFLOAT3& a_vAddition);

				DX12Transform2D();

				/// <summary>
				/// Sets the position of the transform.
				/// </summary>
				/// <param name="a_vPosition">A XMFLOAT3 containing the position.</param>
				void SetPosition(const DirectX::XMFLOAT3& a_vPosition);

				/// <summary>
				/// Sets the rotation of the transform.
				/// </summary>
				/// <param name="a_vRotation">A vector containing the rotation in degrees.</param>
				void SetRotation(const DirectX::XMVECTOR& a_vRotation); // Rotation in degrees

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
				/// <returns>A XMFLOAT3 containing the position.</returns>
				const DirectX::XMFLOAT3& GetPosition() const;

				/// <summary>
				/// Retrieves the rotation of the transform.
				/// </summary>
				/// <returns>A vector containing the rotation.</returns>
				const DirectX::XMVECTOR& GetRotationQ() const;

				/// <summary>
				/// Retrieves the rotation of the transform.
				/// </summary>
				/// <returns>A vector containing the rotation in degrees.</returns>
				DirectX::XMFLOAT3 GetRotationV() const;

				/// <summary>
				/// Retrieves the scale of the transform.
				/// </summary>
				/// <returns>A XMFLOAT3 containing the scale.</returns>
				const DirectX::XMFLOAT3& GetScale() const;

				/// <summary>
				/// Retrieves the pivot of the transform.
				/// </summary>
				/// <returns>A XMFLOAT3 containing the pivot.</returns>
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
			private:
				DirectX::XMFLOAT3 m_vPosition = { 0, 0, 0 };
				DirectX::XMFLOAT3 m_vScale = { 1, 1, 1 };
				DirectX::XMFLOAT3 m_vPivot = { -0.5f, -0.5f, -0.5f };
				DirectX::XMVECTOR m_vRotation = { DirectX::XMQuaternionIdentity() };

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(DX12Transform2D)
				EXPOSE_FIELD(DX12Transform2D, m_vPosition, "Position", (FieldOptions{ .type = EditorFieldWidgetType::Vector3Field, .description = "The position of the object in 2D space. Defines where the object is located on the screen." }))
				EXPOSE_FIELD(DX12Transform2D, m_vScale, "Scale", (FieldOptions{ .type = EditorFieldWidgetType::Vector3Field, .description = "The size multiplier of the object. A value of 1 means default size, values greater than 1 enlarge the object, and values below 1 shrink it." }))
				EXPOSE_FIELD(DX12Transform2D, m_vPivot, "Pivot", (FieldOptions{ .type = EditorFieldWidgetType::Vector3Field, .min = "-0.5", .max = "0.5", .description = "The pivot point for transformations relative to the object�s center. Coordinates represent the normalized offset used for scaling and rotation." }))
				EXPOSE_FIELD(DX12Transform2D, m_vRotation, "Rotation", (FieldOptions{ .type = EditorFieldWidgetType::Quaternion, .description = "Rotation in degrees. Controls how much the object is rotated clockwise or counterclockwise." }))
			END_EXPOSE_FIELDS(DX12Transform2D)
			BEGIN_EXPOSE_GIZMOS(DX12Transform2D)
			END_EXPOSE_GIZMOS(DX12Transform2D)
			END_EXPOSE_TO_EDITOR(DX12Transform2D)
#endif
			};
		}
	}
}