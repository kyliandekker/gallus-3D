#include "Game.h"

// core includes
#include "core/Tool.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/Texture.h"

// gameplay includes
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "gameplay/systems/PlayerSystem.h"

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

			core::TOOL->GetECS().CreateSystem<SpriteSystem>().Initialize();
			core::TOOL->GetECS().CreateSystem<TransformSystem>().Initialize();
			core::TOOL->GetECS().CreateSystem<PlayerSystem>().Initialize();

			core::TOOL->GetWindow().OnQuit() += std::bind(&Game::Shutdown, this);

			System::Initialize();

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

			while (m_bRunning.load())
			{
				auto current = clock::now();
				std::chrono::duration<double> elapsed = current - previous;
				previous = current;
				lag += elapsed.count();

				double deltaTime = elapsed.count(); // in seconds

				// Run fixed updates (catch up if needed)
				while (lag >= FIXED_TIMESTEP)
				{
					bool updateRealtime = m_bStarted && !m_bPaused;

					core::TOOL->GetECS().Update(FIXED_TIMESTEP, updateRealtime);
					lag -= FIXED_TIMESTEP;
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