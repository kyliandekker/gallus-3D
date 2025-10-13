#include "SrcData.h"

#include <rapidjson/utils.h>

namespace gallus
{
	namespace resources
	{
		//---------------------------------------------------------------------
		SrcData::SrcData(const rapidjson::Value& a_Value)
		{
			m_Document.CopyFrom(a_Value, m_Document.GetAllocator());
		}

		//---------------------------------------------------------------------
		int SrcData::GetInt(const std::string& a_sKey) const
		{
			int i = 0;
			rapidjson::GetInt(m_Document, a_sKey, i);
			return i;
		}

		//---------------------------------------------------------------------
		float SrcData::GetFloat(const std::string& a_sKey) const
		{
			float f = 0;
			rapidjson::GetFloat(m_Document, a_sKey, f);
			return f;
		}

		//---------------------------------------------------------------------
		std::string SrcData::GetString(const std::string& a_sKey) const
		{
			std::string str;
			rapidjson::GetString(m_Document, a_sKey, str);
			return str;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetBool(const std::string& a_sKey) const
		{
			bool b = 0;
			rapidjson::GetBool(m_Document, a_sKey, b);
			return b;
		}

		//---------------------------------------------------------------------
		DirectX::XMFLOAT2 SrcData::GetVector(const std::string& a_sKey) const
		{
			float x = 0, y = 0;
			if (m_Document.HasMember(a_sKey.c_str()) && m_Document[a_sKey.c_str()].IsObject())
			{
				rapidjson::GetFloat(m_Document[a_sKey.c_str()], "x", x);
				rapidjson::GetFloat(m_Document[a_sKey.c_str()], "y", y);
			}
			return { x,y };
		}

		//---------------------------------------------------------------------
		SrcData SrcData::GetSrc(const std::string& a_sKey) const
		{
			if (m_Document.HasMember(a_sKey.c_str()) && m_Document[a_sKey.c_str()].IsObject())
			{
				return SrcData(m_Document[a_sKey.c_str()]);
			}
			return SrcData();
		}
	}
}