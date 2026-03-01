#include "Settings.h"

// core
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

// resources
#include "resources/SrcData.h"

// file
#include "file/file_abstractions.h"

namespace gallus::core
{
	//---------------------------------------------------------------------
	// Settings
	//---------------------------------------------------------------------
	Settings::Settings(const std::string& a_sFileName) : m_sFileName(a_sFileName)
	{}

	//---------------------------------------------------------------------
	bool Settings::Load()
	{
		std::string filePath = m_sPath + "/" + m_sFileName;

		core::Data data;
		if (!file::LoadFile(filePath, data))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed loading settings file: Something went wrong when trying to load settings file \"%s\".", m_sFileName.c_str());
			return false;
		}

		DeserializeFields(this, resources::SrcData(data));

		return true;
	}

	//---------------------------------------------------------------------
	bool Settings::Save() const
	{
		std::string filePath = m_sPath + "/" + m_sFileName;

		if (filePath.empty())
		{
			return false;
		}

		resources::SrcData srcData;
		srcData.SetObject();
		SerializeFields(this, srcData);

		core::Data data;
		srcData.GetData(data);
		if (!file::SaveFile(filePath, data))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_GAME, "Failed saving settings file: Something went wrong when trying to save settings file \"%s\".", m_sFileName.c_str());
			return false;
		}

		return true;
	}
}