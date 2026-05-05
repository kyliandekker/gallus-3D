#pragma once

#include "resources/ISerializableObject.h"

// external
#include <DirectXMath.h>

#ifndef _RELEASE
#include <string>
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

namespace gallus::graphics::dx12
{
	enum CameraType
	{
		CameraType_World,
		CameraType_Screen
	};

#ifndef _RELEASE
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
#endif

	//---------------------------------------------------------------------
	// Transform
	//---------------------------------------------------------------------
	class Transform : public ISerializableObject
	{
	public:
		static DirectX::XMFLOAT3 QuaternionToEuler(const DirectX::XMVECTOR & quat);
		static DirectX::XMVECTOR EulerToQuaternion(const DirectX::XMFLOAT3& a_vEuler);
		static DirectX::XMVECTOR AddRotation(DirectX::XMVECTOR& a_vQuat, const DirectX::XMFLOAT3& a_vAddition);
		static DirectX::XMVECTOR AddRotationLocal(DirectX::XMVECTOR& a_vQuat, const DirectX::XMFLOAT3& a_vAddition);

		Transform();

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

		DirectX::XMFLOAT3 m_vPosition = VEC_ZERO;
		DirectX::XMFLOAT3 m_vScale = VEC_IDENTITY;
		DirectX::XMVECTOR m_vRotation = { DirectX::XMQuaternionIdentity() };
		DirectX::XMFLOAT3 m_vPivot = VEC_PIVOT_CENTER;
		graphics::dx12::CameraType m_CameraType = CameraType::CameraType_World;

		BEGIN_SERIALIZE(Transform)
		 	SERIALIZE_FIELD(m_vPosition, "Position", "The position of the object in 2D space. Defines where the object is located on the screen.",
		 		.type = FieldSerializationType::FieldSerializationType_Vector3)
		 	SERIALIZE_FIELD(m_vScale, "Scale", "The size multiplier of the object. A value of 1 means default size, values greater than 1 enlarge the object, and values below 1 shrink it.",
		 		.type = FieldSerializationType::FieldSerializationType_Vector3)
		 	SERIALIZE_FIELD(m_vRotation, "Rotation", "Rotation in degrees. Controls how much the object is rotated clockwise or counterclockwise.",
		 		.type = FieldSerializationType::FieldSerializationType_Quaternion)
		 	SERIALIZE_FIELD(m_vPivot, "Pivot", "The pivot point for transformations relative to the object's center. Coordinates represent the normalized offset used for scaling and rotation.",
		 		.type = FieldSerializationType::FieldSerializationType_Vector3,
		 		.min = "-0.5",
		 		.max = "0.5")
		 	SERIALIZE_FIELD(m_CameraType, "Camera Type", "Whether the camera is rendering 3D or 2D.",
		 		.type = FieldSerializationType::FieldSerializationType_Enum,
		 		.enumToStringFunc = MakeEnumToStringFunc<graphics::dx12::CameraType>(graphics::dx12::CameraTypeToString))
		END_SERIALIZE(Transform)
	};
}