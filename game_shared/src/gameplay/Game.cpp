#include "Game.h"

#include "core/Tool.h"
#include "logger/Logger.h"
#include "gameplay/systems/SpriteSystem.h"
#include "gameplay/systems/TransformSystem.h"
#include "graphics/dx12/CommandQueue.h"
#include "graphics/dx12/CommandList.h"

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
			LOG(gallus::LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading textures.");
			return false;
		}

		if (!LoadSounds())
		{
			LOG(gallus::LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading sounds.");
			return false;
		}

		Test();

		System::Initialize();

		LOG(gallus::LOGSEVERITY_INFO, LOG_CATEGORY_GAME, "Initialized game.");

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
		auto cCommandQueue = gallus::core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto cCommandList = cCommandQueue->GetCommandList();

		auto entityId = gallus::core::TOOL->GetECS().CreateEntity("New Sprite");
		gallus::gameplay::SpriteComponent* meshComp = reinterpret_cast<gallus::gameplay::SpriteComponent*>(gallus::core::TOOL->GetECS().GetSystem<gallus::gameplay::SpriteSystem>().CreateBaseComponent(entityId));
		meshComp->SetMesh(gallus::core::TOOL->GetResourceAtlas().GetDefaultMesh().get());
		meshComp->SetShader(gallus::core::TOOL->GetResourceAtlas().GetDefaultShaderBind().get());
		meshComp->SetTexture(gallus::core::TOOL->GetResourceAtlas().LoadTexture("tex_layton.png", cCommandList).get());
		meshComp->SetSizeToSpriteSize();
		gallus::gameplay::TransformComponent* transformComp = reinterpret_cast<gallus::gameplay::TransformComponent*>(gallus::core::TOOL->GetECS().GetSystem<gallus::gameplay::TransformSystem>().CreateBaseComponent(entityId));
		cCommandQueue->ExecuteCommandList(cCommandList);
		cCommandQueue->Flush();

		return true;
	}
}