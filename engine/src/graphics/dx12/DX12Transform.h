#pragma once

// external
#include <DirectXMath.h>
#include <array>

#include <string>

#ifdef _EDITOR
#include "editor/EditorExpose.h"
#endif

constexpr DirectX::XMVECTOR UP = { 0, 1, 0 }; /// Up vector.
constexpr DirectX::XMFLOAT3 VEC_ZERO = { 0, 0, 0 }; /// Zero vector.
constexpr DirectX::XMFLOAT3 VEC_IDENTITY = { 1, 1, 1 }; /// Identity vector.
constexpr DirectX::XMFLOAT3 VEC_PIVOT_CENTER = { -0.5f, -0.5f, -0.5f }; /// Identity vector.
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
			enum CameraType
			{
				CameraType_World,
				CameraType_Screen
			};

			inline std::string CameraTypeToString(CameraType a_CameraType)
			{
				switch (a_CameraType)
				{
				case CameraType_World:
				{
					return "World";
				}
				case CameraType_Screen:
				{
					return "Screen";
				}
				}
				return "";
			}

			//---------------------------------------------------------------------
			// DX12Transform
			//---------------------------------------------------------------------
			class DX12Transform
#ifdef _EDITOR
				: public IExposableToEditor
#endif
			{
			public:
				static DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMVECTOR & quat);
				static DirectX::XMVECTOR EulerToQuaternion(const DirectX::XMFLOAT3& a_vEuler);
				static DirectX::XMVECTOR AddRotation(DirectX::XMVECTOR& a_vQuat, const DirectX::XMFLOAT3& a_vAddition);

				DX12Transform();

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

				graphics::dx12::CameraType GetCameraType() const
				{
					return m_CameraType;
				}

				void SetCameraMode(graphics::dx12::CameraType a_CameraType)
				{
					m_CameraType = a_CameraType;
				}
			private:
				DirectX::XMFLOAT3 m_vPosition = VEC_ZERO;
				DirectX::XMFLOAT3 m_vScale = VEC_IDENTITY;
				DirectX::XMVECTOR m_vRotation = { DirectX::XMQuaternionIdentity() };
				DirectX::XMFLOAT3 m_vPivot = VEC_PIVOT_CENTER;
				graphics::dx12::CameraType m_CameraType;

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(DX12Transform)
				EXPOSE_FIELD(DX12Transform, m_vPosition, "Position", (FieldOptions{.type = EditorFieldWidgetType::Vector3Field, .description = "The position of the object in 2D space. Defines where the object is located on the screen." }))
				EXPOSE_FIELD(DX12Transform, m_vScale, "Scale", (FieldOptions{.type = EditorFieldWidgetType::Vector3Field, .description = "The size multiplier of the object. A value of 1 means default size, values greater than 1 enlarge the object, and values below 1 shrink it." }))
				EXPOSE_FIELD(DX12Transform, m_vRotation, "Rotation", (FieldOptions{.type = EditorFieldWidgetType::Quaternion, .description = "Rotation in degrees. Controls how much the object is rotated clockwise or counterclockwise." }))
				EXPOSE_FIELD(DX12Transform, m_vPivot, "Pivot", (FieldOptions{.type = EditorFieldWidgetType::Vector3Field, .min = "-0.5", .max = "0.5", .description = "The pivot point for transformations relative to the object�s center. Coordinates represent the normalized offset used for scaling and rotation." }))
				EXPOSE_FIELD(DX12Transform, m_CameraType, "Camera Type",
					(FieldOptions{
						.type = EditorFieldWidgetType::EnumDropdown,
						.enumToStringFunc = MakeEnumToStringFunc<graphics::dx12::CameraType>(graphics::dx12::CameraTypeToString),
						.description = "Whether the camera is rendering 3D or 2D."
					}))
			END_EXPOSE_FIELDS(DX12Transform)
			BEGIN_EXPOSE_GIZMOS(DX12Transform)
			END_EXPOSE_GIZMOS(DX12Transform)
			END_EXPOSE_TO_EDITOR(DX12Transform)
#endif
			};
		}
	}
}