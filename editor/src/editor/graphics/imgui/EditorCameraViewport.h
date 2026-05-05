#pragma once

#include "graphics/dx12/MeshRenderData.h"

namespace gallus::graphics::dx12
{
	class Camera;
	class RenderView;
}
namespace gallus::graphics::imgui
{
	enum class ViewportType
	{
		ViewportType_Preview = 1,
		ViewportType_CameraPreview = 2,
	};

	class EditorCameraViewport
	{
	public:
		void Initialize();

		graphics::dx12::MeshRenderData& GetPreviewData()
		{
			return m_PreviewData;
		}
		graphics::dx12::MeshRenderData& GetCameraPreviewData()
		{
			return m_CameraPreviewData;
		}
		std::shared_ptr<graphics::dx12::RenderView> GetCameraPreviewView()
		{
			return m_pCameraPreviewRenderView;
		}
	private:
		std::unique_ptr<graphics::dx12::Camera> m_pPreviewCamera;
		graphics::dx12::MeshRenderData m_PreviewData;
		std::shared_ptr<graphics::dx12::RenderView> m_pPreviewRenderView;

		graphics::dx12::MeshRenderData m_CameraPreviewData;
		std::shared_ptr<graphics::dx12::RenderView> m_pCameraPreviewRenderView;
	};
}