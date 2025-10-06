#pragma once

#include <string>
#include <rapidjson/document.h>

namespace gallus
{
	namespace resources
	{
		class SrcData
		{
		public:
			SrcData(rapidjson::Document& a_Doc);

			int GetInt(const std::string& a_sKey);
			float GetFloat(const std::string& a_sKey);
			template <typename T>
			T GetEnum(const std::string& a_sKey)
			{
				return static_cast<T>(GetInt(a_sKey));
			}
			std::string GetString(const std::string& a_sKey);
			bool GetBool(const std::string& a_sKey);
		private:
			rapidjson::Document m_Document;
		};
	}
}