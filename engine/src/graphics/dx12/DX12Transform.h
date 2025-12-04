#pragma once

// external
#include <DirectXMath.h>
#include <array>

#ifdef _EDITOR
#include "editor/EditorExpose.h"
#endif

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
#ifdef _EDITOR
				: public IExposableToEditor
#endif
			{
			public:
				DX12Transform();

				/// <summary>
				/// Sets the position of the transform.
				/// </summary>
				/// <param name="a_vPosition">A XMFLOAT2 containing the position.</param>
				void SetPosition(const DirectX::XMFLOAT2& a_vPosition);

				/// <summary>
				/// Sets the rotation of the transform.
				/// </summary>
				/// <param name="a_fRotationDegrees">A float containing the rotation in degrees.</param>
				void SetRotation(float a_fRotationDegrees); // Rotation in degrees

				/// <summary>
				/// Sets the scale of the transform.
				/// </summary>
				/// <param name="a_vScale">A XMFLOAT2 containing the scale.</param>
				void SetScale(const DirectX::XMFLOAT2& a_vScale);

				/// <summary>
				/// Sets the pivot of the transform.
				/// </summary>
				/// <param name="a_vPivot">A XMFLOAT2 containing the pivot.</param>
				void SetPivot(const DirectX::XMFLOAT2& a_vPivot);

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

				/// <summary>
				/// Retrieves the pivot of the transform.
				/// </summary>
				/// <returns>A XMFLOAT2 containing the pivot.</returns>
				const DirectX::XMFLOAT2& GetPivot() const;

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
				DirectX::XMFLOAT2 m_vPosition = { 0, 0 };
				DirectX::XMFLOAT2 m_vScale = { 1, 1 };
				DirectX::XMFLOAT2 m_vPivot = { -0.5f, -0.5f };
				float m_fRotationDegrees = 0.0f; // rotation around Z axis

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(DX12Transform)
				EXPOSE_FIELD(DX12Transform, m_vPosition, "Position", (FieldOptions{ .type = EditorFieldWidgetType::Vector2Field, .description = "The position of the object in 2D space. Defines where the object is located on the screen." }))
				EXPOSE_FIELD(DX12Transform, m_vScale, "Scale", (FieldOptions{ .type = EditorFieldWidgetType::Vector2Field, .description = "The size multiplier of the object. A value of 1 means default size, values greater than 1 enlarge the object, and values below 1 shrink it." }))
				EXPOSE_FIELD(DX12Transform, m_vPivot, "Pivot", (FieldOptions{ .type = EditorFieldWidgetType::Vector2Field, .min = "-0.5", .max = "0.5", .description = "The pivot point for transformations relative to the object�s center. Coordinates represent the normalized offset used for scaling and rotation." }))
				EXPOSE_FIELD(DX12Transform, m_fRotationDegrees, "Rotation", (FieldOptions{ .type = EditorFieldWidgetType::DragFloat, .description = "Rotation around the Z-axis in degrees. Controls how much the object is rotated clockwise or counterclockwise." }))
			END_EXPOSE_FIELDS(DX12Transform)
			BEGIN_EXPOSE_GIZMOS(DX12Transform)
			END_EXPOSE_GIZMOS(DX12Transform)
			END_EXPOSE_TO_EDITOR(DX12Transform)
#endif
			};
		}
	}
}