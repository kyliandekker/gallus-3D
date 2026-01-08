#include "Settings.h"

// external
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core
#include "core/Engine.h"
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

// resources
#include "resources/SrcData.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// Settings
		//---------------------------------------------------------------------
		Settings::Settings(const std::string& a_sFileName) : m_sFileName(a_sFileName)
		{}

		//---------------------------------------------------------------------
		bool Settings::Load()
		{
			const fs::path path = ENGINE->GetSaveDirectory().generic_string() + "/" + m_sFileName;
			core::Data data;
			if (!file::LoadFile(path, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to load settings file \"%s\".", path.generic_string().c_str());
				return false;
			}

			DeserializeFields(this, resources::SrcData(data));

			return true;
		}

		//---------------------------------------------------------------------
		bool Settings::Save() const
		{
			if (m_sFileName.empty())
			{
				return false;
			}

			resources::SrcData srcData;
			srcData.SetObject();
			SerializeFields(this, srcData);

			const fs::path path = ENGINE->GetSaveDirectory().generic_string() + "/" + m_sFileName;

			core::Data data;
			srcData.GetData(data);
			if (!file::SaveFile(path, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Something went wrong when trying to save settings file \"%s\".", path.generic_string().c_str());
				return false;
			}

			return true;
		}
	}
}