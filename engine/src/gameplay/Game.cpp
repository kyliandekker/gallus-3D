#include "Game.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/win32/Window.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/systems/MeshSystem.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/ProjectileSystem.h"
#include "gameplay/systems/AnimationSystem.h"
#include "gameplay/systems/RigidbodySystem.h"

#include "graphics/dx12/RenderView.h"
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/HeapAllocation.h"
#include "graphics/dx12/Camera.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/DirectionalLight.h"
#include "graphics/dx12/CommandList.h"

#include "resources/ResourceAtlas.h"

namespace gallus::gameplay
{
     static Game s_Game;
     Game& GetGame() { return s_Game; }

	//---------------------------------------------------------------------
	// Game
	//---------------------------------------------------------------------
	bool Game::Initialize()
	{
		LOG(LOGSEVERITY_INFO, LOG_CATEGORY_GAME, "Initializing game.");

		m_Scene.SetResourceCategory(resources::EngineResourceCategory::Game);

		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

		ecs->CreateSystem<MeshSystem>()->Initialize();
		ecs->CreateSystem<SpriteSystem>()->Initialize();
		ecs->CreateSystem<TransformSystem>()->Initialize();
		ecs->CreateSystem<HealthSystem>()->Initialize();
		ecs->CreateSystem<CollisionSystem>()->Initialize();
		ecs->CreateSystem<ProjectileSystem>()->Initialize();
		ecs->CreateSystem<AnimationSystem>()->Initialize();
		ecs->CreateSystem<RigidbodySystem>()->Initialize();

		graphics::win32::Window* window = GetEngine().GetWindow();
		if (!window)
		{
			return false;
		}

		window->OnQuit() += std::bind(&Game::Shutdown, this);

		m_bStarted = false;

		std::shared_ptr<graphics::dx12::RenderView> renderView = GetDX12System().RegisterView(0);
		if (renderView)
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

			renderView->m_iRTVIndex = GetDX12System().GetRTV().Allocate();
			renderView->m_iDSVIndex = GetDX12System().GetDSV().Allocate();
			GetDX12System().SetDepthBuffer(renderView->m_pDepthBuffer);

			renderView->m_pRenderTarget = GetResourceAtlas().LoadTextureByDescription("RenderTexture", texDesc);

			if (std::shared_ptr<graphics::dx12::Texture> texture = renderView->m_pRenderTarget.lock())
			{
				texture->SetResourceCategory(resources::EngineResourceCategory::System);
				texture->SetIsDestroyable(false);

				GetDX12System().GetDevice()->CreateRenderTargetView(texture->GetResource().Get(), nullptr, GetDX12System().GetRTV().GetCPUHandle(renderView->m_iRTVIndex));
			}

			renderView->m_Viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
			renderView->m_ScissorRect = CD3DX12_RECT(0, 0, graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);
			
			renderView->m_RenderFunc = [renderView](std::shared_ptr<graphics::dx12::CommandQueue> pCommandQueue, std::shared_ptr<graphics::dx12::CommandList> pCommandList)
			{
				renderView->m_pCamera = &GetDX12System().GetActiveCamera();
				if (!renderView->m_pCamera)
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
					pair.second->Render(pCommandList, pair.first, *renderView->m_pCamera);
				}
			};
		}

		System::Initialize();

		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_GAME, "Successfully initialized game.");

		return true;
	}

	//---------------------------------------------------------------------
	bool Game::Destroy()
	{
		return false;
	}

	//---------------------------------------------------------------------
	void Game::Loop()
	{
		m_FpsCounter.Initialize();
		m_FpsCounter.SetTargetFPS(60); 
		m_FpsCounter.m_eOnNewFrame += std::bind(&Game::NewFrame, this, std::placeholders::_1);

		while (m_bRunning.load())
		{
			m_FpsCounter.Update();
		}
	}

	//---------------------------------------------------------------------
	void Game::NewFrame(float a_fDeltaTime)
	{
		m_eOnNewFrame(m_FpsCounter.GetFPS());

		bool updateRealtime = m_bStarted && !m_bPaused;
		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return;
		}

		ecs->Update(a_fDeltaTime, updateRealtime);

		if (updateRealtime)
		{
			// TODO: Game shared code.
		}
	}

	//---------------------------------------------------------------------
	void Game::Shutdown()
	{
		m_bRunning.store(false);
	}
}