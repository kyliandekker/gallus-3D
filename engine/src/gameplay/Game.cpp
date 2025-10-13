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
#include "gameplay/systems/MovementSystem.h"
#include "gameplay/systems/ProjectileSystem.h"
#include "gameplay/systems/AnimationSystem.h"

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
			core::ENGINE->GetECS().CreateSystem<MovementSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<ProjectileSystem>().Initialize();
			core::ENGINE->GetECS().CreateSystem<AnimationSystem>().Initialize();

			core::ENGINE->GetWindow().OnQuit() += std::bind(&Game::Shutdown, this);

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
		constexpr double FIXED_TIMESTEP = 1.0 / 60.0; // 60 FPS -> 16.66 ms
		void Game::Loop()
		{
			using clock = std::chrono::high_resolution_clock;
			auto previous = clock::now();
			double lag = 0.0;

			double fpsTimer = 0.0;
			int fpsFrames = 0;

			while (m_bRunning.load())
			{
				auto current = clock::now();
				std::chrono::duration<double> elapsed = current - previous;
				previous = current;

				lag += elapsed.count();
				fpsTimer += elapsed.count();

				int updatesThisFrame = 0;

				m_fDeltaTime = FIXED_TIMESTEP;
				while (lag >= FIXED_TIMESTEP)
				{
					bool updateRealtime = m_bStarted && !m_bPaused;
					core::ENGINE->GetECS().Update(FIXED_TIMESTEP, updateRealtime);
					lag -= FIXED_TIMESTEP;
					updatesThisFrame++;

					if (updateRealtime)
					{
						const gameplay::Entity* player = core::ENGINE->GetECS().GetEntityByName("Player");

						if (player)
						{
							gameplay::TransformSystem& transformSys = core::ENGINE->GetECS().GetSystem<gameplay::TransformSystem>();
							gameplay::TransformComponent& transformComponent = transformSys.GetComponent(player->GetEntityID());
							core::ENGINE->GetDX12().GetActiveCamera().Transform().SetPosition(transformComponent.Transform().GetPosition());
						}
					}
				}

				fpsFrames += updatesThisFrame;

				if (fpsTimer >= 1.0)
				{
					// m_Fps reflects how many fixed updates we actually managed per second
					m_fFps = static_cast<double>(fpsFrames) / fpsTimer;
					fpsFrames = 0;
					fpsTimer = 0.0;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}

		//---------------------------------------------------------------------
		void Game::Shutdown()
		{
			m_bRunning.store(false);
		}
	}
}