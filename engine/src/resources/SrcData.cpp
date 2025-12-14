#include "SrcData.h"

// external
#include <rapidjson/utils.h>

#include "logger/Logger.h"

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
		bool SrcData::GetInt(const std::string& a_sKey, int& a_iInt) const
		{
			if (m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!rapidjson::GetInt(m_Document, a_sKey, a_iInt))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s is not present or not an integer.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetFloat(const std::string& a_sKey, float& a_fFloat) const
		{
			if (m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!rapidjson::GetFloat(m_Document, a_sKey, a_fFloat))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s is not present or not a float.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetString(const std::string& a_sKey, std::string& a_sString) const
		{
			if (m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!rapidjson::GetString(m_Document, a_sKey, a_sString))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s is not present or not a string.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetBool(const std::string& a_sKey, bool& a_bBool) const
		{
			if (m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!rapidjson::GetBool(m_Document, a_sKey, a_bBool))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s is not present or not a boolean.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetVector2(const std::string& a_sKey, DirectX::XMFLOAT2& a_vVector) const
		{
			if (m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s is not present or not an object/vector.", a_sKey.c_str());
				return false;
			}

			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], "x", a_vVector.x))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s does not have an x axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], "y", a_vVector.y))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s does not have an y axis.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetVector3(const std::string& a_sKey, DirectX::XMFLOAT3& a_vVector) const
		{
			if (m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s is not present or not an object/vector.", a_sKey.c_str());
				return false;
			}

			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], "x", a_vVector.x))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s does not have an x axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], "y", a_vVector.y))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s does not have an y axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], "z", a_vVector.z))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s does not have an z axis.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetSrc(const std::string& a_sKey, SrcData& a_SrcData) const
		{
			if (m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key %s is not present or not an object.", a_sKey.c_str());
				return false;
			}

			a_SrcData = SrcData(m_Document[a_sKey.c_str()]);

			return true;
		}
	}
}