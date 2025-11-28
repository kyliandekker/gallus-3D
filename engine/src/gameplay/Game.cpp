// header
#include "Game.h"

// core includes
#include "core/Engine.h"

// graphics includes
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// logger includes
#include "logger/Logger.h"

// gameplay includes
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/PlayerSystem.h"
#include "gameplay/systems/HealthSystem.h"
#include "gameplay/systems/CollisionSystem.h"
#include "gameplay/systems/ProjectileSystem.h"
#include "gameplay/systems/AnimationSystem.h"
#include "gameplay/systems/RigidbodySystem.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// Game
		//---------------------------------------------------------------------
		bool Game::Initialize()
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_GAME, "Initializing game.");

			m_Scene.SetResourceCategory(resources::EngineResourceCategory::Game);

			core::ENGINE->GetECS().CreateSystem<SpriteSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<TransformSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<PlayerSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<HealthSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<CollisionSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<ProjectileSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<AnimationSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<RigidbodySystem>().Initialize();

			core::ENGINE->GetWindow().OnQuit() += std::bind(&Game::Shutdown, this);

			m_bStarted = false;

			System::Initialize();

#ifndef _EDITOR
			core::ENGINE->GetResourceAtlas().LoadScene("main.scene", m_Scene);
			m_Scene.LoadData();
			m_bStarted = true;
#endif

			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_GAME, "Initialized game.");

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
			core::ENGINE->GetECS().Update(a_fDeltaTime, updateRealtime);

			if (updateRealtime)
			{
				const gameplay::Entity* player = core::ENGINE->GetECS().GetEntityByName("Player");

				if (player)
				{
					gameplay::TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<gameplay::TransformSystem>();
					gameplay::TransformComponent& transformComponent = transformSys.GetComponent(player->GetEntityID());
					DirectX::XMFLOAT2 pos = { transformComponent.Transform().GetPosition().x - (graphics::dx12::RENDER_TEX_SIZE.x / 2), transformComponent.Transform().GetPosition().y - (graphics::dx12::RENDER_TEX_SIZE.y / 2) };
					core::ENGINE->GetDX12().GetActiveCamera().Transform().SetPosition(pos);
				}
			}
		}

		//---------------------------------------------------------------------
		void Game::Shutdown()
		{
			m_bRunning.store(false);
		}
	}
}