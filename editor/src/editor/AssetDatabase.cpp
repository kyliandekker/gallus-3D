#include "AssetDatabase.h"

#include "logger/Logger.h"
#include "core/EditorEngine.h"

namespace gallus
{
	namespace editor
	{
		//---------------------------------------------------------------------
		// AssetDatabase
		//---------------------------------------------------------------------
		bool AssetDatabase::Initialize()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Initialized asset database.");
			return System::Initialize();
		}

		//---------------------------------------------------------------------
		bool AssetDatabase::Destroy()
		{
			LOG(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_EDITOR, "Destroyed asset database.");
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
			if (core::ENGINE->GetResourceAtlas().GetResourceFolder().Scan())
			{
				LOG(LOGSEVERITY_INFO, LOG_CATEGORY_EDITOR, "Scanned asset database.");
				m_eOnScanCompleted();
				return true;
			}

			return false;
		}
	}
}