#pragma once

// external
#include <string>
#include <vector>
#include <rapidjson/document.h>

namespace gallus
{
	class Vector2;
	class IVector2;
	class Vector3;
	class Vector4;
	class Color4;

	namespace core
	{
		class Data;
	}
	namespace resources
	{
		class SrcData
		{
		public:
			SrcData() = default;
			SrcData(const core::Data& a_Data);
			SrcData(const rapidjson::Value& a_Value);

			void DocOrMember(const std::string& a_sKey, rapidjson::Document& a_Document) const;

			// getters
			bool GetInt(const std::string& a_sKey, int32_t& a_iInt) const;
			template <typename T>
			bool GetEnum(const std::string& a_sKey, T& a_Enum) const
			{
				int32_t enumVal = 0;
				bool success = GetInt(a_sKey, enumVal);
				a_Enum = static_cast<T>(enumVal);
				return success;
			}
			bool GetBool(const std::string& a_sKey, bool& a_bBool) const;
			bool GetFloat(const std::string& a_sKey, float& a_fFloat) const;

			bool GetString(const std::string& a_sKey, std::string& a_sString) const;
			bool GetVector2(const std::string& a_sKey, Vector2& a_vVector) const;
			bool GetIVector2(const std::string& a_sKey, IVector2& a_vVector) const;
			bool GetVector3(const std::string& a_sKey, Vector3& a_vVector) const;
			bool GetVector4(const std::string& a_sKey, Vector4& a_vVector) const;
			bool GetColor(const std::string& a_sKey, Color4& a_vVector) const;

			bool GetSrcObject(const std::string& a_sKey, SrcData& a_SrcData) const;
			bool HasSrcObject(const std::string& a_sKey) const;

			bool GetSrcArray(const std::string& a_sKey, SrcData& a_SrcData) const;
			bool GetArraySize(size_t& a_iSize) const;
			bool GetSrcArrayElement(size_t a_iIndex, SrcData& a_SrcData) const;
			bool GetAllMemberNames(std::vector<std::string>& a_aMembers) const;

			// setters
			void SetObject();
			void SetArray();
			void SetInt(const std::string& a_sKey, int32_t a_iInt);
			void SetBool(const std::string& a_sKey, bool a_bBool);
			template <typename T>
			void SetEnum(const std::string& a_sKey, T a_Enum)
			{
				SetInt(a_sKey, static_cast<int32_t>(a_Enum));
			}
			void SetFloat(const std::string& a_sKey, float a_fFloat);

			void SetString(const std::string& a_sKey, const std::string& a_sString);
			void SetVector2(const std::string& a_sKey, const Vector2& a_vVector);
			void SetIVector2(const std::string& a_sKey, const IVector2& a_vVector);
			void SetVector3(const std::string& a_sKey, const Vector3& a_vVector);
			void SetVector4(const std::string& a_sKey, const Vector4& a_vVector);
			void SetColor(const std::string& a_sKey, const Color4& a_vVector);

			void SetSrcObject(const std::string& a_sKey, SrcData& a_SrcData);
			void PushArraySrcObject(SrcData& a_SrcData);

			// utils
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

			const std::string& GetName() const
			{
				return m_sName;
			}

			void GetData(core::Data& a_Data) const;
		private:
			bool m_bIsValid = false;
			std::string m_sName;
			rapidjson::Document m_Document;
		};
	}
}