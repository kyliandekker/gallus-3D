#include "SrcData.h"

#include <rapidjson/utils.h>

namespace gallus
{
	namespace resources
	{
		SrcData::SrcData(rapidjson::Document& a_Doc)
		{
			m_Document.CopyFrom(a_Doc, a_Doc.GetAllocator());
		}

		int SrcData::GetInt(const std::string& a_sKey)
		{
			int i = 0;
			rapidjson::GetInt(m_Document, a_sKey, i);
			return i;
		}

		float SrcData::GetFloat(const std::string& a_sKey)
		{
			float f = 0;
			rapidjson::GetFloat(m_Document, a_sKey, f);
			return f;
		}

		std::string SrcData::GetString(const std::string& a_sKey)
		{
			std::string str;
			rapidjson::GetString(m_Document, a_sKey, str);
			return str;
		}

		bool SrcData::GetBool(const std::string& a_sKey)
		{
			bool b = 0;
			rapidjson::GetBool(m_Document, a_sKey, b);
			return b;
		}
	}
}