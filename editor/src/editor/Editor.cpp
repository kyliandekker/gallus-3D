#include "Editor.h"

#include <iostream>

#include "graphics/dx12/DX12System2D.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace editor
	{
		//---------------------------------------------------------------------
		// Editor
		//---------------------------------------------------------------------
		Editor::Editor() : core::ThreadedSystem::ThreadedSystem(), 
			m_EditorSettings("editorsettings.settings")
		{}

		//---------------------------------------------------------------------
		bool Editor::Initialize(bool a_bWait)
		{
			m_CurrentScene.SetResourceCategory(resources::EngineResourceCategory::Editor);
			m_EditorCamera.Init(graphics::dx12::RENDER_TEX_SIZE.x, graphics::dx12::RENDER_TEX_SIZE.y);

			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Initializing editor.");

			return ThreadedSystem::Initialize(a_bWait);
		}

		//---------------------------------------------------------------------
		void Editor::Loop()
		{
			std::lock_guard<std::mutex> lock(m_EditorMutex);
			m_AssetDatabase.CheckAssetDatabase();
		}

		//---------------------------------------------------------------------
		bool Editor::Destroy()
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Destroying editor.");
			return ThreadedSystem::Destroy();
		}

		//---------------------------------------------------------------------
		bool Editor::InitThreadWorker()
		{
			m_AssetDatabase.Initialize();

			m_EditorSettings.Load();

			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized editor.");

			return true;
		}

		//---------------------------------------------------------------------
		void Editor::Finalize()
		{
			m_AssetDatabase.Destroy();

			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed editor.");
		}
	}
}