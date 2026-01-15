#include "SrcData.h"

// external
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core
#include "core/Data.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/math.h"

namespace gallus
{
	namespace resources
	{
		#define SRC_DATA_VECTOR_X "x"
		#define SRC_DATA_VECTOR_Y "y"
		#define SRC_DATA_VECTOR_Z "z"
		#define SRC_DATA_VECTOR_W "w"

		#define SRC_DATA_VECTOR_R "r"
		#define SRC_DATA_VECTOR_G "g"
		#define SRC_DATA_VECTOR_B "b"
		#define SRC_DATA_VECTOR_A "a"

		//---------------------------------------------------------------------
		SrcData::SrcData(const rapidjson::Value& a_Value)
		{
			m_Document.CopyFrom(a_Value, m_Document.GetAllocator());

			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Something went wrong when trying to load src data.");
			}
			else
			{
				m_bIsValid = true;
			}
		}

		//---------------------------------------------------------------------
		void SrcData::DocOrMember(const std::string& a_sKey, rapidjson::Document& a_Document) const
		{
			if (a_sKey.empty())
			{
				a_Document.CopyFrom(m_Document, a_Document.GetAllocator());
			}
			else
			{
				if (m_Document.IsObject() && m_Document.HasMember(a_sKey.c_str()))
				{
					a_Document.CopyFrom(m_Document[a_sKey.c_str()], a_Document.GetAllocator());
				}
				else
				{
					a_Document.SetObject();
				}
			}
		}

		//---------------------------------------------------------------------
		SrcData::SrcData(const core::Data& a_Data)
		{
			m_Document.Parse(a_Data.dataAs<const char>(), a_Data.size());

			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_RESOURCES, "Something went wrong when trying to load src data.");
			}
			else
			{
				m_bIsValid = true;
			}
		}

		//---------------------------------------------------------------------
		bool SrcData::GetInt(const std::string& a_sKey, int32_t& a_iInt) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			rapidjson::Document doc;
			DocOrMember(a_sKey, doc);
			if (!rapidjson::GetInt(doc, a_iInt))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an integer.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetBool(const std::string& a_sKey, bool& a_bBool) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			rapidjson::Document doc;
			DocOrMember(a_sKey, doc);
			if (!rapidjson::GetBool(doc, a_bBool))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not a boolean.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetFloat(const std::string& a_sKey, float& a_fFloat) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			rapidjson::Document doc;
			DocOrMember(a_sKey, doc);
			if (!rapidjson::GetFloat(doc, a_fFloat))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not a float.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetString(const std::string& a_sKey, std::string& a_sString) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			rapidjson::Document doc;
			DocOrMember(a_sKey, doc);
			if (!rapidjson::GetString(doc, a_sString))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not a string.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetVector2(const std::string& a_sKey, Vector2& a_vVector) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an object/vector.", a_sKey.c_str());
				return false;
			}
			
			rapidjson::Document doc;
			DocOrMember(a_sKey, doc);
			if (!rapidjson::GetFloat(doc, SRC_DATA_VECTOR_X, a_vVector.x))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an x axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(doc, SRC_DATA_VECTOR_Y, a_vVector.y))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an y axis.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetIVector2(const std::string& a_sKey, IVector2& a_vVector) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an object/vector.", a_sKey.c_str());
				return false;
			}

			rapidjson::Document doc;
			DocOrMember(a_sKey, doc);
			if (!rapidjson::GetInt(doc, SRC_DATA_VECTOR_X, a_vVector.x))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an x axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetInt(doc, SRC_DATA_VECTOR_Y, a_vVector.y))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an y axis.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetVector3(const std::string& a_sKey, Vector3& a_vVector) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an object/vector.", a_sKey.c_str());
				return false;
			}

			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_X, a_vVector.x))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an x axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_Y, a_vVector.y))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an y axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_Z, a_vVector.z))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an z axis.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetVector4(const std::string& a_sKey, Vector4& a_vVector) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an object/vector.", a_sKey.c_str());
				return false;
			}

			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_X, a_vVector.x))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an x axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_Y, a_vVector.y))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an y axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_Z, a_vVector.z))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an z axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_W, a_vVector.w))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an w axis.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetColor(const std::string& a_sKey, Color4& a_vVector) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an object/vector.", a_sKey.c_str());
				return false;
			}

			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_R, a_vVector.r))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an r axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_G, a_vVector.g))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an g axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_B, a_vVector.b))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an b axis.", a_sKey.c_str());
				return false;
			}
			if (!rapidjson::GetFloat(m_Document[a_sKey.c_str()], SRC_DATA_VECTOR_A, a_vVector.a))
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" does not have an a axis.", a_sKey.c_str());
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetSrcObject(const std::string& a_sKey, SrcData& a_SrcData) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an object.", a_sKey.c_str());
				return false;
			}

			a_SrcData = SrcData(m_Document[a_sKey.c_str()]);
			return a_SrcData.IsValid();
		}

		//---------------------------------------------------------------------
		bool SrcData::HasSrcObject(const std::string& a_sKey) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsObject())
			{
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetSrcArray(const std::string& a_sKey, SrcData& a_SrcData) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.HasMember(a_sKey.c_str()) || !m_Document[a_sKey.c_str()].IsArray())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Key \"%s\" is not present or not an array.", a_sKey.c_str());
				return false;
			}

			a_SrcData = SrcData(m_Document[a_sKey.c_str()]);

			if (!a_SrcData.IsValid())
			{
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetArraySize(size_t& a_iSize) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.IsArray())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is not an array.");
				return false;
			}
			
			a_iSize = m_Document.GetArray().Size();

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetSrcArrayElement(size_t a_iIndex, SrcData& a_SrcData) const
		{
			if (m_Document.HasParseError() || m_Document.IsNull())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is empty.");
				return false;
			}

			if (!m_Document.IsArray())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Document is not an array.");
				return false;
			}

			if (a_iIndex >= m_Document.GetArray().Size())
			{
				LOGF(LogSeverity::LOGSEVERITY_WARNING, LOG_CATEGORY_RESOURCES, "Provided index exceeds array size.");
				return false;
			}

			a_SrcData = SrcData(m_Document.GetArray()[a_iIndex]);

			if (!a_SrcData.IsValid())
			{
				return false;
			}

			return true;
		}

		//---------------------------------------------------------------------
		bool SrcData::GetAllMemberNames(std::vector<std::string>& a_aMembers) const
		{
			a_aMembers.reserve(m_Document.MemberCount());
			for (auto it = m_Document.MemberBegin(); it != m_Document.MemberEnd(); ++it)
			{
				a_aMembers.push_back(it->name.GetString());
			}
			return true;
		}

		//---------------------------------------------------------------------
		void SrcData::SetObject()
		{
			m_Document.SetObject();
		}

		//---------------------------------------------------------------------
		void SrcData::SetArray()
		{
			m_Document.SetArray();
		}

		//---------------------------------------------------------------------
		void SrcData::SetInt(const std::string& a_sKey, int32_t a_iInt)
		{
			if (a_sKey.empty())
			{
				m_Document.SetInt(a_iInt);
			}
			else
			{
				rapidjson::SetOrAddMember(m_Document, a_sKey.c_str(), a_iInt, m_Document.GetAllocator());
			}
		}

		//---------------------------------------------------------------------
		void SrcData::SetBool(const std::string& a_sKey, bool a_bBool)
		{
			if (a_sKey.empty())
			{
				m_Document.SetBool(a_bBool);
			}
			else
			{
				rapidjson::SetOrAddMemberBool(m_Document, a_sKey.c_str(), a_bBool, m_Document.GetAllocator());
			}
		}

		//---------------------------------------------------------------------
		void SrcData::SetFloat(const std::string& a_sKey, float a_fFloat)
		{
			if (a_sKey.empty())
			{
				m_Document.SetFloat(a_fFloat);
			}
			else
			{
				rapidjson::SetOrAddMemberFloat(m_Document, a_sKey.c_str(), a_fFloat, m_Document.GetAllocator());
			}
		}

		//---------------------------------------------------------------------
		void SrcData::SetVector2(const std::string& a_sKey, const Vector2& a_vVector)
		{
			rapidjson::Document::AllocatorType& allocator = m_Document.GetAllocator();
			rapidjson::Value vectorObj(rapidjson::kObjectType);

			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_X, allocator).Move(), a_vVector.x, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_Y, allocator).Move(), a_vVector.y, allocator);

			m_Document.AddMember(rapidjson::Value(a_sKey.c_str(), allocator).Move(), vectorObj, allocator);
		}

		//---------------------------------------------------------------------
		void SrcData::SetIVector2(const std::string& a_sKey, const IVector2& a_vVector)
		{
			rapidjson::Document::AllocatorType& allocator = m_Document.GetAllocator();
			rapidjson::Value vectorObj(rapidjson::kObjectType);

			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_X, allocator).Move(), a_vVector.x, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_Y, allocator).Move(), a_vVector.y, allocator);

			m_Document.AddMember(rapidjson::Value(a_sKey.c_str(), allocator).Move(), vectorObj, allocator);
		}

		//---------------------------------------------------------------------
		void SrcData::SetVector3(const std::string& a_sKey, const Vector3& a_vVector)
		{
			rapidjson::Document::AllocatorType& allocator = m_Document.GetAllocator();
			rapidjson::Value vectorObj(rapidjson::kObjectType);

			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_X, allocator).Move(), a_vVector.x, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_Y, allocator).Move(), a_vVector.y, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_Z, allocator).Move(), a_vVector.z, allocator);

			m_Document.AddMember(rapidjson::Value(a_sKey.c_str(), allocator).Move(), vectorObj, allocator);
		}

		//---------------------------------------------------------------------
		void SrcData::SetVector4(const std::string& a_sKey, const Vector4& a_vVector)
		{
			rapidjson::Document::AllocatorType& allocator = m_Document.GetAllocator();
			rapidjson::Value vectorObj(rapidjson::kObjectType);

			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_X, allocator).Move(), a_vVector.x, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_Y, allocator).Move(), a_vVector.y, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_Z, allocator).Move(), a_vVector.z, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_W, allocator).Move(), a_vVector.w, allocator);

			m_Document.AddMember(rapidjson::Value(a_sKey.c_str(), allocator).Move(), vectorObj, allocator);
		}

		//---------------------------------------------------------------------
		void SrcData::SetColor(const std::string& a_sKey, const Color4& a_vVector)
		{
			rapidjson::Document::AllocatorType& allocator = m_Document.GetAllocator();
			rapidjson::Value vectorObj(rapidjson::kObjectType);

			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_R, allocator).Move(), a_vVector.r, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_G, allocator).Move(), a_vVector.g, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_B, allocator).Move(), a_vVector.b, allocator);
			vectorObj.AddMember(rapidjson::Value(SRC_DATA_VECTOR_A, allocator).Move(), a_vVector.a, allocator);

			m_Document.AddMember(rapidjson::Value(a_sKey.c_str(), allocator).Move(), vectorObj, allocator);
		}

		//---------------------------------------------------------------------
		void SrcData::SetSrcObject(const std::string& a_sKey, SrcData& a_SrcData)
		{
			rapidjson::SetOrAddMember(m_Document, a_sKey.c_str(), a_SrcData.GetDocument());
		}

		//---------------------------------------------------------------------
		void SrcData::PushArraySrcObject(SrcData& a_SrcData)
		{
			rapidjson::Value copy;
			copy.CopyFrom(a_SrcData.GetDocument(), m_Document.GetAllocator()); // deep copy
			m_Document.PushBack(copy, m_Document.GetAllocator());
		}

		//---------------------------------------------------------------------
		void SrcData::SetString(const std::string& a_sKey, const std::string& a_sString)
		{
			if (a_sKey.empty())
			{
				m_Document.SetString(a_sString.c_str(), a_sString.size());
			}
			else
			{
				rapidjson::SetOrAddMember(m_Document, a_sKey.c_str(), a_sString.c_str(), m_Document.GetAllocator());
			}
		}

		//---------------------------------------------------------------------
		void SrcData::GetData(core::Data& a_Data) const
		{
			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			m_Document.Accept(writer);

			a_Data = core::Data(buffer.GetString(), buffer.GetSize());
		}
	}
}