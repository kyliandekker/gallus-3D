#include "./EditorCameraViewport.h"

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/RenderView.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/HeapAllocation.h"
#include "graphics/dx12/DirectionalLight.h"
#include "graphics/dx12/ShaderFactory.h"

#include "gameplay/systems/MeshSystem.h"

// resources
#include "resources/ResourceAtlas.h"

// editor
#include "editor/core/EditorEngine.h"

namespace gallus::graphics::imgui
{
	void ResetMeshRenderData(graphics::dx12::MeshRenderData& a_MeshRenderData)
	{
		resources::ResourceAtlas* resourceAtlas = GetEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		a_MeshRenderData.m_pTexture = resourceAtlas->GetDefaultTexture();
		a_MeshRenderData.m_pMesh = resourceAtlas->LoadMesh("sphere");
		a_MeshRenderData.m_pMaterial = resourceAtlas->GetDefaultMaterial();
		a_MeshRenderData.m_pPixelShader = resourceAtlas->GetDefaultPixelShader();
		a_MeshRenderData.m_pVertexShader = resourceAtlas->GetDefaultVertexShader();
		a_MeshRenderData.m_iSkinnedMeshIndex = 0;

		a_MeshRenderData.m_pPipelineState = graphics::dx12::PipelineStateCache::GetOrCreate(a_MeshRenderData.m_pPixelShader, a_MeshRenderData.m_pVertexShader, DXGI_FORMAT_D32_FLOAT);
		a_MeshRenderData.m_pRootSignature = GetDX12System().GetRootSignature().Get();
	}

	void EditorCameraViewport::Initialize()
	{
		m_pPreviewCamera = std::make_unique<graphics::dx12::Camera>();

		m_pPreviewCamera->Init(graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
		m_pPreviewCamera->GetTransform().SetPosition({ 0.0f, 0, -1.0f });

		m_pPreviewRenderView = GetDX12System().RegisterView((int)ViewportType::ViewportType_Preview);
		if (m_pPreviewRenderView)
		{
			D3D12_RESOURCE_DESC texDesc = {};
			texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			texDesc.Width = graphics::dx12::RENDER_TEX_SIZE.x;
			texDesc.Height = graphics::dx12::RENDER_TEX_SIZE.y;
			texDesc.DepthOrArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

			m_pPreviewRenderView->m_iRTVIndex = GetDX12System().GetRTV().Allocate();
			m_pPreviewRenderView->m_iDSVIndex = GetDX12System().GetDSV().Allocate();
			GetDX12System().SetDepthBuffer(m_pPreviewRenderView->m_pDepthBuffer, m_pPreviewRenderView->m_iDSVIndex);

			m_pPreviewRenderView->m_pRenderTarget = GetResourceAtlas().LoadTextureByDescription("EditorRenderTexture", texDesc);

			m_pPreviewRenderView->m_pCamera = m_pPreviewCamera.get();

			if (std::shared_ptr<graphics::dx12::Texture> texture = m_pPreviewRenderView->m_pRenderTarget.lock())
			{
				texture->SetResourceCategory(resources::EngineResourceCategory::System);
				texture->SetIsDestroyable(false);

				GetDX12System().GetDevice()->CreateRenderTargetView(texture->GetResource().Get(), nullptr, GetDX12System().GetRTV().GetCPUHandle(m_pPreviewRenderView->m_iRTVIndex));
			}

			m_pPreviewRenderView->m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
			m_pPreviewRenderView->m_ScissorRect = CD3DX12_RECT(0, 0, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);

			ResetMeshRenderData(m_PreviewData);

			m_pPreviewRenderView->m_RenderFunc = [this](std::shared_ptr<graphics::dx12::CommandQueue> pCommandQueue, std::shared_ptr<graphics::dx12::CommandList> pCommandList)
			{
				if (std::shared_ptr<graphics::dx12::DirectionalLight> dirLight = GetDX12System().GetDirectionalLight().lock())
				{
					dirLight->Bind(pCommandList);
				}

				graphics::dx12::Transform transform;
				m_PreviewData.Render(pCommandList, transform, 0, *m_pPreviewRenderView->m_pCamera);
			};
		}

		m_pCameraPreviewRenderView = GetDX12System().RegisterView((int)ViewportType::ViewportType_CameraPreview);
		if (m_pCameraPreviewRenderView)
		{
			D3D12_RESOURCE_DESC texDesc = {};
			texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			texDesc.Width = graphics::dx12::RENDER_TEX_SIZE.x;
			texDesc.Height = graphics::dx12::RENDER_TEX_SIZE.y;
			texDesc.DepthOrArraySize = 1;
			texDesc.MipLevels = 1;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc.Count = 1;
			texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

			m_pCameraPreviewRenderView->m_iRTVIndex = GetDX12System().GetRTV().Allocate();
			m_pCameraPreviewRenderView->m_iDSVIndex = GetDX12System().GetDSV().Allocate();
			GetDX12System().SetDepthBuffer(m_pCameraPreviewRenderView->m_pDepthBuffer, m_pCameraPreviewRenderView->m_iDSVIndex);

			m_pCameraPreviewRenderView->m_pRenderTarget = GetResourceAtlas().LoadTextureByDescription("EditorRenderTexture", texDesc);

			m_pCameraPreviewRenderView->m_pCamera = m_pPreviewCamera.get();

			if (std::shared_ptr<graphics::dx12::Texture> texture = m_pCameraPreviewRenderView->m_pRenderTarget.lock())
			{
				texture->SetResourceCategory(resources::EngineResourceCategory::System);
				texture->SetIsDestroyable(false);

				GetDX12System().GetDevice()->CreateRenderTargetView(texture->GetResource().Get(), nullptr, GetDX12System().GetRTV().GetCPUHandle(m_pCameraPreviewRenderView->m_iRTVIndex));
			}

			m_pCameraPreviewRenderView->m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
			m_pCameraPreviewRenderView->m_ScissorRect = CD3DX12_RECT(0, 0, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);

			ResetMeshRenderData(m_CameraPreviewData);

			m_pCameraPreviewRenderView->m_RenderFunc = [this](std::shared_ptr<graphics::dx12::CommandQueue> pCommandQueue, std::shared_ptr<graphics::dx12::CommandList> pCommandList)
			{
				if (!m_pCameraPreviewRenderView->m_pCamera)
				{
					return;
				}

				gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
				if (!ecs)
				{
					return;
				}

				if (!ecs->Running())
				{
					return;
				}

				if (std::shared_ptr<graphics::dx12::DirectionalLight> dirLight = GetDX12System().GetDirectionalLight().lock())
				{
					dirLight->Bind(pCommandList);
				}

				gameplay::MeshSystem* meshSys = ecs->GetSystem<gameplay::MeshSystem>();
				if (!meshSys)
				{
					return;
				}
				for (auto& pair : meshSys->GetComponents())
				{
					pair.second->Render(pCommandList, pair.first, *m_pCameraPreviewRenderView->m_pCamera);
				}
			};
		}
	}
}