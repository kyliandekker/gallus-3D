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

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// Settings
		//---------------------------------------------------------------------
		Settings::Settings(const std::string& a_sFileName) : m_sFileName(a_sFileName)
		{

		}

		//---------------------------------------------------------------------
		bool Settings::Load()
		{
			DataStream data;
			const fs::path path = ENGINE->GetSaveDirectory().generic_string() + "/" + m_sFileName;

			if (!file::LoadFile(path, data))
			{
				Save();
				return false;
			}

			rapidjson::Document document;
			document.Parse(reinterpret_cast<char*>(data.data()), data.size());

			if (document.HasParseError())
			{
				Save();
				return false;
			}

			if (!LoadVars(document))
			{
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool Settings::Save() const
		{
			rapidjson::Document document;
			document.SetObject();
			rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

			if (!SaveVars(document, allocator))
			{
				return false;
			}

			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			document.Accept(writer);

			fs::path path = ENGINE->GetSaveDirectory().generic_string() + "/" + m_sFileName;
			file::CreateDirectory(path.parent_path());
			if (!file::SaveFile(path, core::Data(buffer.GetString(), buffer.GetSize())))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_CORE, "Something went wrong when trying to save the settings: \"%s\".", path.filename().generic_string().c_str());
				return false;
			}

			return true;
		}
	}
}