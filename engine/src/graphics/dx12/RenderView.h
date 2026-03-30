#pragma once

#include "graphics/dx12/DX12PCH.h"

#include <memory>
#include <functional>

namespace gallus::graphics::dx12
{
	class Texture;
	class DX12Resource;
	class Camera;
	class CommandQueue;
	class CommandList;

	class RenderView
	{
	public:
		std::weak_ptr<Texture> m_pRenderTarget;
		std::unique_ptr<DX12Resource> m_pDepthBuffer;

		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

		Camera* m_pCamera = nullptr;

		uint32_t m_iRTVIndex = UINT32_MAX;
		uint32_t m_iDSVIndex = UINT32_MAX;

		std::function<void(std::shared_ptr<CommandQueue>, std::shared_ptr<CommandList>)> m_RenderFunc;
	};
}