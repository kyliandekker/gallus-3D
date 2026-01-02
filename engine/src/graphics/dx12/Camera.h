#pragma once

#include "DX12PCH.h"

// external
#include <DirectXMath.h>

#include "core/Observable.h"

// graphics
#include "graphics/dx12/Transform.h"

#include "editor/ISerializableObject.h"

namespace gallus
{
	namespace resources
	{
		class SrcData;
	}
	namespace graphics
	{
		namespace dx12
		{
			constexpr DirectX::XMFLOAT2 RENDER_TEX_SIZE = { 1920, 1080 };
#ifdef _EDITOR
			enum CameraMode
			{
				CameraMode_Editor,
				CameraMode_Game
			};
#endif // _EDITOR

			//---------------------------------------------------------------------
			// Camera
			//---------------------------------------------------------------------
			class Camera : public ISerializableObject
			{
			public:
				/// <summary>
				/// Constructs a camera with an identity transform and no projection set.
				/// </summary>
				Camera();

				/// <summary>
				/// Initializes the camera with a default projection based on the given width and height.
				/// Near and far plane values are set to defaults.
				/// </summary>
				/// <param name="a_fWidth">The width of the rendering viewport.</param>
				/// <param name="a_fHeight">The height of the rendering viewport.</param>
				void Init(float a_fWidth, float a_fHeight);

				/// <summary>
				/// Sets the projection matrix of the camera using perspective parameters.
				/// </summary>
				void SetProjection();

				/// <summary>
				/// Gets a reference to the camera's transform, allowing modification of position and orientation.
				/// </summary>
				/// <returns>Reference to the transform component.</returns>
				Transform& GetTransform();

				/// <summary>
				/// Gets a constant reference to the camera's transform.
				/// </summary>
				/// <returns>Constant reference to the transform component.</returns>
				const Transform& GetTransform() const;

				/// <summary>
				/// Computes and returns the view matrix from the camera's transform.
				/// </summary>
				/// <returns>The view matrix used for rendering from the camera's perspective.</returns>
				DirectX::XMMATRIX GetViewMatrix(CameraType a_CameraType) const;

				/// <summary>
				/// Returns the current projection matrix of the camera.
				/// </summary>
				/// <returns>The projection matrix used for rendering.</returns>
				const DirectX::XMMATRIX& GetProjectionMatrix(CameraType a_CameraType) const;

				/// <summary>
				/// Returns the size of the camera.
				/// </summary>
				/// <returns>Vector representing the size of the camera.</returns>
				const DirectX::XMINT2& GetSize() const
				{
					return m_vSize;
				}

				const std::string GetName() const
				{
					return "Transform";
				}

				void SetFoV(float a_fFoV)
				{
					m_fFoV = a_fFoV;
				}

				float GetFoV() const
				{
					return m_fFoV;
				}

				void OnFoVChanged();
			protected:
				float m_fFoV;
				DirectX::XMINT2 m_vSize;
				Transform m_Transform;
				DirectX::XMMATRIX m_ProjectionMatrix3D;
				DirectX::XMMATRIX m_ProjectionMatrix2D;

				BEGIN_SERIALIZE(Camera)
				 	SERIALIZE_FIELD(m_Transform.m_vPosition, "Position", "The position of the object in 2D space. Defines where the object is located on the screen.",
				 		.type = FieldSerializationType::FieldSerializationType_Vector3)
				 	SERIALIZE_FIELD(m_Transform.m_vRotation, "Rotation", "Rotation in degrees. Controls how much the object is rotated clockwise or counterclockwise.",
				 		.type = FieldSerializationType::FieldSerializationType_Quaternion)
				 	SERIALIZE_FIELD(m_Transform.m_vPivot, "Pivot", "The pivot point for transformations relative to the object's center. Coordinates represent the normalized offset used for scaling and rotation.",
				 		.type = FieldSerializationType::FieldSerializationType_Vector3,
				 		.min = "-0.5",
				 		.max = "0.5")
				 	SERIALIZE_FIELD(m_fFoV, "Field of View", "",
				 		.type = FieldSerializationType::FieldSerializationType_Float,
				 		.onChangeFunc = MakeOnChangeFunc(&Camera::OnFoVChanged))
				END_SERIALIZE(Camera)
			};
		}
	}
}