#pragma once

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

			int GetInt(const std::string& a_sKey) const;
			float GetFloat(const std::string& a_sKey) const;
			template <typename T>
			T GetEnum(const std::string& a_sKey) const
			{
				return static_cast<T>(GetInt(a_sKey));
			}
			std::string GetString(const std::string& a_sKey) const;
			bool GetBool(const std::string& a_sKey) const;
			DirectX::XMFLOAT2 GetVector(const std::string& a_sKey) const;
			SrcData GetSrc(const std::string& a_sKey) const;
		private:
			rapidjson::Document m_Document;
		};
	}
}