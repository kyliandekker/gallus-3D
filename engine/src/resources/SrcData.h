#pragma once

// external
#include <string>
#include <rapidjson/document.h>
#include <DirectXMath.h>

namespace gallus
{
	namespace resources
	{
		class SrcData
		{
		public:
			SrcData() = default;
			SrcData(const rapidjson::Value& a_Value);

			bool GetInt(const std::string& a_sKey, int& a_iInt) const;
			bool GetFloat(const std::string& a_sKey, float& a_fFloat) const;
			template <typename T>
			bool GetEnum(const std::string& a_sKey, T& a_Enum) const
			{
				int enumVal = 0;
				bool success = GetInt(a_sKey, enumVal);
				a_Enum = static_cast<T>(enumVal);
				return success;
			}
			bool GetString(const std::string& a_sKey, std::string& a_sString) const;
			bool GetBool(const std::string& a_sKey, bool& a_bBool) const;
			bool GetVector2(const std::string& a_sKey, DirectX::XMFLOAT2& a_vVector) const;
			bool GetVector3(const std::string& a_sKey, DirectX::XMFLOAT3& a_vVector) const;
			bool GetSrc(const std::string& a_sKey, SrcData& a_SrcData) const;
		private:
			rapidjson::Document m_Document;
		};
	}
}