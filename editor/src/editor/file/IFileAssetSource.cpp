#include "./IFileAssetSource.h"

// core
#include "core/Data.h"

// logger
#include "logger/Logger.h"

// file
#include "file/file_abstractions.h"

// utils
#include "utils/string_extensions.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/AssetDatabase.h"

// editor/file
#include "editor/file/FileResource.h"

namespace gallus::file
{
	//---------------------------------------------------------------------
	// IFileAssetSource
	//---------------------------------------------------------------------
	bool IFileAssetSource::LoadBinary(const std::string& a_sID, core::Data& a_OutData)
	{
		editor::AssetDatabase* assetDatabase = GetEditorEngine().GetAssetDatabase();
		if (!assetDatabase)
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary data for asset \"%s\" because asset database was null.", a_sID.c_str());
			return false;
		}

		file::FileResource* fileResource = nullptr;

		std::string id = string_extensions::StringToLower(a_sID);
		if (!assetDatabase->GetResourceFolder()->Find(id, fileResource))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary data for asset \"%s\" because it could not find the asset.", a_sID.c_str());
			return false;
		}

		if (!fileResource)
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary data for asset \"%s\" because the asset was null.", a_sID.c_str());
			return false;
		}

		if (!fileResource->GetFileData(a_OutData))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary data for asset \"%s\" because it could not fetch file data.", a_sID.c_str());
			return false;
		}

		if (a_OutData.empty())
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary data for asset \"%s\" because data was empty.", a_sID.c_str());
			return false;
		}

		return true;
	}

	//---------------------------------------------------------------------
	bool IFileAssetSource::LoadMetaData(const std::string& a_sID, bool a_bSeparate, core::Data& a_OutData)
	{
		editor::AssetDatabase* assetDatabase = GetEditorEngine().GetAssetDatabase();
		if (!assetDatabase)
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary meta data for asset \"%s\" because asset database was null.", a_sID.c_str());
			return false;
		}

		file::FileResource* fileResource = nullptr;

		std::string id = string_extensions::StringToLower(a_sID);
		if (!assetDatabase->GetResourceFolder()->Find(id, fileResource))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary data for asset \"%s\" because it could not find the asset.", a_sID.c_str());
			return false;
		}

		if (!fileResource)
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary data for asset \"%s\" because the asset was null.", a_sID.c_str());
			return false;
		}

		if (!file::LoadFile(fileResource->GetPath().generic_string() + ".meta", a_OutData))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary meta data for asset \"%s\" because something went wrong with loading the meta data file.", a_sID.c_str());
			return false;
		}

		if (a_OutData.empty())
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Failed loading binary meta data for asset \"%s\" because data was empty.", a_sID.c_str());
			return false;
		}

		return true;
	}
}