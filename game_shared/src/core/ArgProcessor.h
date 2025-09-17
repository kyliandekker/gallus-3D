#include <string>
#include <vector>

#pragma once

constexpr const char* ASSET_PATH_ARG = "assetPath";

namespace gallus
{
	namespace core
	{
		class BaseArg
		{
		public:
			BaseArg(const std::string& a_sName);

			virtual const std::string& GetName() const
			{
				return m_sName;
			}
		protected:
			std::string m_sName;
		};

		template<typename T>
		class Arg : public BaseArg
		{
		public:
			Arg(const std::string& a_sName, const T& a_Value) : BaseArg(a_sName), m_Value(a_Value)
			{}

			const T& GetValue() const
			{
				return m_Value;
			}
		protected:
			T m_Value;
		};

		class ArgProcessor
		{
		public:
			static std::vector<BaseArg*> GetArguments(const std::string& a_sArgs);
		};
	}
}