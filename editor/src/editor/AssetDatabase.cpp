#include "AssetDatabase.h"

// core
#include "core/EditorEngine.h"
#include "core/ArgProcessor.h"

// logger
#include "logger/Logger.h"

// file
#include "editor/file/FileResource.h"

namespace gallus::editor
{
	//---------------------------------------------------------------------
	// AssetDatabase
	//---------------------------------------------------------------------
	bool AssetDatabase::Initialize(bool a_bWait)
	{
		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Initializing asset database.");

		return ThreadedSystem::Initialize(a_bWait);
	}

	//---------------------------------------------------------------------
	bool AssetDatabase::Destroy()
	{
		m_bRescan = false;

		LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Destroying asset database.");
		return System::Destroy();
	}

	//---------------------------------------------------------------------
	void AssetDatabase::Rescan()
	{
		m_bRescan = true;
	}

	//---------------------------------------------------------------------
	void AssetDatabase::CheckAssetDatabase()
	{
		if (m_bRescan)
		{
			std::lock_guard<std::recursive_mutex> lock(m_AssetMutex);
			if (Scan())
			{
				m_bRescan = false;
			}
		}
	}

	//---------------------------------------------------------------------
	bool AssetDatabase::Scan()
	{
		if (!m_pResourceFolder)
		{
			return false;
		}

		if (m_pResourceFolder->Scan())
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Scanned asset database.");
			m_eOnScanCompleted();
			return true;
		}

		return false;
	}

	//---------------------------------------------------------------------
	void AssetDatabase::Loop()
	{
		CheckAssetDatabase();
	}

	//---------------------------------------------------------------------
	bool AssetDatabase::Sleep() const
	{
		// TODO: Use m_bLoadAssets or something in AssetDatabase to wake up thread and let it sleep otherwise.
		return true;
	}

	//---------------------------------------------------------------------
	bool AssetDatabase::InitThreadWorker()
	{
		std::string resourceFolder = core::ARGS.GetArgument<std::string>(ASSET_PATH_ARG);
		if (resourceFolder.empty())
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_ECS, "Assets folder has not been set. Please add the argument: \"assetPath=\"your/path/to/assets/folder\"\".");
			return false;
		}

		m_pResourceFolder = new file::FileResource();
		m_pResourceFolder->SetPath(resourceFolder);
		m_pResourceFolder->Scan();

		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized asset database.");

		return true;
	}

	//---------------------------------------------------------------------
	void AssetDatabase::Finalize()
	{
		LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed asset database.");
	}
}