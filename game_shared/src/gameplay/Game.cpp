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

			core::TOOL->GetWindow().OnQuit() += std::bind(&Game::Shutdown, this);

			if (!LoadTextures())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading textures.");
				return false;
			}

			if (!LoadSounds())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading sounds.");
				return false;
			}

			Test();

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
				if (m_bStarted)
				{
					auto current = clock::now();
					std::chrono::duration<double> elapsed = current - previous;
					previous = current;
					lag += elapsed.count();

					// Run fixed updates (catch up if needed)
					while (lag >= FIXED_TIMESTEP)
					{
						core::TOOL->GetECS().Update(FIXED_TIMESTEP);
						lag -= FIXED_TIMESTEP;
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}

		//---------------------------------------------------------------------
		void Game::Shutdown()
		{
			m_bRunning.store(false);
		}

		//---------------------------------------------------------------------
		bool Game::LoadTextures()
		{
			return true;
		}

		//---------------------------------------------------------------------
		bool Game::LoadSounds()
		{
			return true;
		}

		//---------------------------------------------------------------------
		bool Game::Test()
		{
			/*
			auto cCommandQueue = gallus::core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
			auto cCommandList = cCommandQueue->GetCommandList();

			auto entityId = gallus::core::TOOL->GetECS().CreateEntity("New Sprite");
			gallus::gameplay::SpriteComponent* meshComp = reinterpret_cast<gallus::gameplay::SpriteComponent*>(gallus::core::TOOL->GetECS().GetSystem<gallus::gameplay::SpriteSystem>().CreateBaseComponent(entityId));
			meshComp->SetMesh(gallus::core::TOOL->GetResourceAtlas().GetDefaultMesh().get());
			meshComp->SetShader(gallus::core::TOOL->GetResourceAtlas().GetDefaultShaderBind().get());
			auto tex = gallus::core::TOOL->GetResourceAtlas().LoadTexture("tex_layton_sprites.png", cCommandList);
			tex->AddSpriteRect({ 0, 0, 74, 190 });
			tex->SetCurrentSprite(0);
			meshComp->SetTexture(tex.get());
			gallus::gameplay::TransformComponent* transformComp = reinterpret_cast<gallus::gameplay::TransformComponent*>(gallus::core::TOOL->GetECS().GetSystem<gallus::gameplay::TransformSystem>().CreateBaseComponent(entityId));
			transformComp->Transform().SetScale({ static_cast<float>(meshComp->GetTexture()->GetResourceDesc().Width), static_cast<float>(meshComp->GetTexture()->GetResourceDesc().Height) });
			cCommandQueue->ExecuteCommandList(cCommandList);
			cCommandQueue->Flush();*/

			return true;
		}
	}
}