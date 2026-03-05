#pragma once

// external
#include <vector>
#include <string>
#include <rapidjson/document.h>

namespace gallus
{
	class Vector2;
	class IVector2;
	class Vector3;
	class Vector4;
	class Color4;
}
namespace gallus::core
{
	class Data;
}
namespace gallus::resources
{
	class SrcData
	{
	public:
		SrcData() = default;
		SrcData(const core::Data& a_Data);
		SrcData(const rapidjson::Value& a_Value);

		//----------- Getters -----------
		// Single types.
		bool GetBool(bool& a_bBool, const std::string a_sPropertyName = "") const;
		bool GetInt(int32_t& a_iInt, const std::string& a_sPropertyName = "") const;
		template <typename T>
		bool GetEnum(T& a_Enum, const std::string& a_sPropertyName = "") const
		{
			int32_t enumVal = 0;
			bool success = GetInt(enumVal, a_sPropertyName);
			a_Enum = static_cast<T>(enumVal);
			return success;
		}
		bool GetFloat(float& a_fFloat, const std::string& a_sPropertyName = "") const;
		bool GetString(std::string& a_sString, const std::string& a_sPropertyName = "") const;

		// Complex types.
		bool GetVector2(Vector2& a_vVector, const std::string& a_sPropertyName = "") const;
		bool GetIVector2(IVector2& a_vVector, const std::string& a_sPropertyName = "") const;
		bool GetVector3(Vector3& a_vVector, const std::string& a_sPropertyName = "") const;
		bool GetVector4(Vector4& a_vVector, const std::string& a_sPropertyName = "") const;
		bool GetColor(Color4& a_vVector, const std::string& a_sPropertyName = "") const;

		bool GetSrcObject(const std::string& a_sKey, SrcData& a_SrcData) const;
		bool HasSrcObject(const std::string& a_sKey) const;

		bool GetSrcArray(const std::string& a_sKey, SrcData& a_SrcData) const;
		bool GetArraySize(size_t& a_iSize) const;
		bool GetSrcArrayElement(size_t a_iIndex, SrcData& a_SrcData) const;
		bool GetAllMemberNames(std::vector<std::string>& a_aMembers) const;

		//----------- Setters -----------
		// Single types.
		void SetInt(const std::string& a_sKey, int32_t a_iInt);
		void SetBool(const std::string& a_sKey, bool a_bBool);
		template <typename T>
		void SetEnum(const std::string& a_sKey, T a_Enum)
		{
			SetInt(a_sKey, static_cast<int32_t>(a_Enum));
		}
		void SetFloat(const std::string& a_sKey, float a_fFloat);
		void SetString(const std::string& a_sKey, const std::string& a_sString);

		// Complex types.
		void SetVector2(const std::string& a_sKey, const Vector2& a_vVector);
		void SetIVector2(const std::string& a_sKey, const IVector2& a_vVector);
		void SetVector3(const std::string& a_sKey, const Vector3& a_vVector);
		void SetVector4(const std::string& a_sKey, const Vector4& a_vVector);
		void SetColor(const std::string& a_sKey, const Color4& a_vVector);

		void SetSrcObject(const std::string& a_sKey, SrcData& a_SrcData);
		void PushArraySrcObject(SrcData& a_SrcData);

		// Utils.
		bool IsObject() const
		{
			return m_Document.IsObject();
		}

		bool IsArray() const
		{
			return m_Document.IsArray();
		}

		void SetObject()
		{
			m_Document.SetObject();
		}

		void SetArray()
		{
			m_Document.SetArray();
		}

		bool IsValid() const
		{
			return m_bIsValid;
		}

		const rapidjson::Document& GetDocument() const
		{
			return m_Document;
		}

		rapidjson::Document& GetDocument()
		{
			return m_Document;
		}

		void GetData(core::Data& a_Data) const;
	private:
		bool m_bIsValid = false;
		rapidjson::Document m_Document;
	};
}