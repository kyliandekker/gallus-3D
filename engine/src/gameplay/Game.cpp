#include "Game.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/systems/MeshSystem.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"
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

			gameplay::EntityComponentSystem& ecs = core::ENGINE->GetECS();
			ecs.CreateSystem<MeshSystem>().Initialize();
			ecs.CreateSystem<SpriteSystem>().Initialize();
			ecs.CreateSystem<TransformSystem>().Initialize();
			ecs.CreateSystem<HealthSystem>().Initialize();
			ecs.CreateSystem<CollisionSystem>().Initialize();
			ecs.CreateSystem<ProjectileSystem>().Initialize();
			ecs.CreateSystem<AnimationSystem>().Initialize();
			ecs.CreateSystem<RigidbodySystem>().Initialize();

			core::ENGINE->GetWindow().OnQuit() += std::bind(&Game::Shutdown, this);

			m_bStarted = false;

			System::Initialize();

#ifndef _EDITOR
			core::ENGINE->GetResourceAtlas().LoadScene("main.scene", m_Scene);
			m_Scene.LoadData();
			m_bStarted = true;
#endif // _EDITOR

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
				// TODO: Game shared code.
			}
		}

		//---------------------------------------------------------------------
		void Game::Shutdown()
		{
			m_bRunning.store(false);
		}
	}
}