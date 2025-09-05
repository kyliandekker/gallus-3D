#include "MetaData.h"

#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core includes
#include "core/DataStream.h"

// utils includes
#include "utils/file_abstractions.h"

// logger includes
#include "logger/Logger.h"

// editor includes
#include "editor/FileResource.h"

#define JSON_META_DATA_FILE_RESOURCE_ASSET_TYPE_VAR "assetType"

namespace gallus
{
	namespace editor
	{
		//---------------------------------------------------------------------
		// MetaData
		//---------------------------------------------------------------------
		MetaData::MetaData(FileResource& a_FileResource) :
			m_FileResource(a_FileResource)
		{}

		//---------------------------------------------------------------------
		AssetType MetaData::GetAssetType() const
		{
			return m_AssetType;
		}

		//---------------------------------------------------------------------
		void MetaData::SetAssetType(AssetType a_AssetType)
		{
			m_AssetType = a_AssetType;
		}

		rapidjson::Document MetaData::Load() const
		{
			rapidjson::Document document = rapidjson::Document();
			document.SetObject();

			core::DataStream data;
			fs::path metaPath = m_FileResource.GetPath().generic_string() + ".meta";
			if (!file::LoadFile(metaPath, data))
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed loading meta file \"%s\".", metaPath.generic_string().c_str());
				return document;
			}

			document.Parse(reinterpret_cast<char*>(data.data()), data.size());

			if (document.HasParseError())
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed loading data in meta file \"%s\".", metaPath.generic_string().c_str());
				return document;
			}

			return document;
		}

		//---------------------------------------------------------------------
		rapidjson::Document MetaData::GenerateMetaData() const
		{
			rapidjson::Document doc = Load();
			
			int assetType = (int) m_AssetType;
			rapidjson::SetOrAddMember(doc, JSON_META_DATA_FILE_RESOURCE_ASSET_TYPE_VAR, assetType, doc.GetAllocator());
			
			return doc;
		}
		
		//---------------------------------------------------------------------
		bool MetaData::Save() const
		{
			fs::path metaPath = m_FileResource.GetPath().generic_string() + ".meta";

			rapidjson::Document doc = GenerateMetaData();
			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			doc.Accept(writer);

			return file::SaveFile(metaPath, core::Data(buffer.GetString(), buffer.GetSize()));
		}

		//---------------------------------------------------------------------
		bool MetaData::LoadMetaData(rapidjson::Document& a_Document)
		{
			rapidjson::Document doc = Load();

			int iAssetType = 0;
			rapidjson::GetInt(doc, JSON_META_DATA_FILE_RESOURCE_ASSET_TYPE_VAR, iAssetType);
			m_AssetType = static_cast<AssetType>(iAssetType);

			return true;
		}

		//---------------------------------------------------------------------
		bool MetaData::Exists() const
		{
			fs::path metaPath = m_FileResource.GetPath().generic_string() + ".meta";
			return fs::exists(metaPath) && !fs::is_directory(metaPath);
		}
	}
}