#pragma once

#include "DX12PCH.h"

// external
#include <DirectXMath.h>

// graphics
#include "graphics/dx12/Transform.h"

#ifdef _EDITOR
#include "editor/EditorExpose.h"
#endif

namespace gallus
{
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
			class Camera
#ifdef _EDITOR
				: public IExposableToEditor
#endif
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
			protected:
				float m_fFov = 60;
				DirectX::XMINT2 m_vSize;
				Transform m_Transform;
				DirectX::XMMATRIX m_ProjectionMatrix3D;
				DirectX::XMMATRIX m_ProjectionMatrix2D;
#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(Camera)
				EXPOSE_FIELD(Camera, m_Transform.m_vPosition, "Position", (FieldOptions{ .type = EditorFieldWidgetType::Vector3Field, .description = "The position of the object in 2D space. Defines where the object is located on the screen." }))
				EXPOSE_FIELD(Camera, m_Transform.m_vRotation, "Rotation", (FieldOptions{ .type = EditorFieldWidgetType::Quaternion, .description = "Rotation in degrees. Controls how much the object is rotated clockwise or counterclockwise." }))
				EXPOSE_FIELD(Camera, m_Transform.m_vPivot, "Pivot", (FieldOptions{ .type = EditorFieldWidgetType::Vector3Field, .min = "-0.5", .max = "0.5", .description = "The pivot point for transformations relative to the object's center. Coordinates represent the normalized offset used for scaling and rotation." }))
				EXPOSE_FIELD(Camera, m_fFov, "Field of View", (FieldOptions{ .type = EditorFieldWidgetType::DragFloat }))
			END_EXPOSE_FIELDS(Camera)
			BEGIN_EXPOSE_GIZMOS(Camera)
			END_EXPOSE_GIZMOS(Camera)
			END_EXPOSE_TO_EDITOR(Camera)
#endif
			};
		}
	}
}