#include "Game.h"

#include "core/Tool.h"
#include "logger/Logger.h"
#include "gameplay/systems/SpriteSystem.h"

namespace game
{
	//---------------------------------------------------------------------
	// Game
	//---------------------------------------------------------------------
	bool Game::Initialize()
	{
		LOG(gallus::LOGSEVERITY_INFO, LOG_CATEGORY_GAME, "Initializing game.");

		gallus::core::TOOL->GetWindow().OnQuit() += std::bind(&Game::Shutdown, this);

		if (!LoadTextures())
		{
			LOG(gallus::LOGSEVERITY_ERROR, LOG_CATEGORY_ENGINE, "Failed loading textures.");
			return false;
		}

		if (!LoadSounds())
		{
			LOG(gallus::LOGSEVERITY_ERROR, LOG_CATEGORY_ENGINE, "Failed loading sounds.");
			return false;
		}

		Test();

		System::Initialize();

		LOG(gallus::LOGSEVERITY_INFO, LOG_CATEGORY_ENGINE, "Initialized game.");

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
		while (m_bRunning.load())
		{
			gallus::core::TOOL->GetECS().Update(0.0f);
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
		auto entityId = gallus::core::TOOL->GetECS().CreateEntity("New Sprite");
		gallus::gameplay::SpriteComponent* meshComp = reinterpret_cast<gallus::gameplay::SpriteComponent*>(gallus::core::TOOL->GetECS().GetSystem<gallus::gameplay::SpriteSystem>().CreateBaseComponent(entityId));
		meshComp->SetMesh(gallus::core::TOOL->GetResourceAtlas().GetDefaultMesh());
		meshComp->SetShader(gallus::core::TOOL->GetResourceAtlas().GetDefaultShader());
		meshComp->SetTexture(gallus::core::TOOL->GetResourceAtlas().GetDefaultTexture());

		return true;
	}
}