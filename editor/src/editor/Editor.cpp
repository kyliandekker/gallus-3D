// header
#include "Editor.h"

// external
#include <iostream>

// core
#include "core/Engine.h"

// logger
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
			m_CurrentScene.SetResourceCategory(core::EngineResourceCategory::Editor);

			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Initializing editor.");

			m_EditorCamera.Init(core::ENGINE->GetDX12().GetCamera().GetSize().x, core::ENGINE->GetDX12().GetCamera().GetSize().y);

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
			m_AssetDatabase.Scan();

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