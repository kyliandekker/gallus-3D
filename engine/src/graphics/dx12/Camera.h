#pragma once

#include "DX12PCH.h"

#include <DirectXMath.h>

// graphics includes
#include "graphics/dx12/DX12Transform.h"

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
				/// <param name="a_fWidth">The width of the rendering viewport.</param>
				/// <param name="a_fHeight">The height of the rendering viewport.</param>
				/// <param name="a_fNearPlane">The near clipping plane distance.</param>
				/// <param name="a_fFarPlane">The far clipping plane distance.</param>
				void SetProjection(float a_fWidth, float a_fHeight, float a_fNearPlane, float a_fFarPlane);

				/// <summary>
				/// Gets a reference to the camera's transform, allowing modification of position and orientation.
				/// </summary>
				/// <returns>Reference to the transform component.</returns>
				DX12Transform& Transform();

				/// <summary>
				/// Gets a constant reference to the camera's transform.
				/// </summary>
				/// <returns>Constant reference to the transform component.</returns>
				const DX12Transform& Transform() const;

				/// <summary>
				/// Computes and returns the view matrix from the camera's transform.
				/// </summary>
				/// <returns>The view matrix used for rendering from the camera's perspective.</returns>
				DirectX::XMMATRIX GetViewMatrix() const;

				/// <summary>
				/// Returns the current projection matrix of the camera.
				/// </summary>
				/// <returns>The projection matrix used for rendering.</returns>
				const DirectX::XMMATRIX& GetProjectionMatrix() const;

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
			private:
				DirectX::XMINT2 m_vSize;
				DX12Transform m_Transform;
				DirectX::XMMATRIX m_ProjectionMatrix;
			};
		}
	}
}