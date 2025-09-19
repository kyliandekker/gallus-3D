#include <string>
#include <vector>

#pragma once

constexpr const char* ASSET_PATH_ARG = "assetPath";
constexpr const char* ASSERT_LEVEL_ARG = "assertLevel";
constexpr const char* LOG_TO_FILE_ARG = "logToFile";
constexpr const char* LOG_TYPE_ARG = "logType";

namespace gallus
{
	namespace core
	{
		class BaseArg
		{
		public:
			BaseArg(const std::string& a_sName) : m_sName(a_sName) {}
			virtual ~BaseArg() = default;

			virtual const std::string& GetName() const { return m_sName; }

			virtual std::string GetValueAsString() const = 0;
			virtual void SetValueFromString(const std::string& str) = 0;

		protected:
			std::string m_sName;
		};
		
		template<typename T>
		class Arg : public BaseArg
		{
		public:
			Arg(const std::string& a_sName, const T& a_Value)
				: BaseArg(a_sName), m_Value(a_Value)
			{}

			const T& GetValue() const { return m_Value; }
			void SetValue(const T& value) { m_Value = value; }

			std::string GetValueAsString() const override
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					return m_Value;
				}
				else if constexpr (std::is_same_v<T, bool>)
				{
					return m_Value ? "true" : "false";
				}
				else
				{
					return std::to_string(m_Value);
				}
			}

			void SetValueFromString(const std::string& str) override
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					m_Value = str;
				}
				else if constexpr (std::is_same_v<T, bool>)
				{
					m_Value = (str == "1" || str == "true");
				}
				else
				{
					m_Value = static_cast<T>(std::stoi(str));
				}
			}
		private:
			T m_Value;
		};

		class ArgProcessor
		{
		public:
			void ProcessArguments(const std::string& a_sArgs);

			template<typename T>
			void AddArgument(const std::string& a_sName, const T& a_Value)
			{
				m_aArgs.push_back(new Arg<T>(a_sName, a_Value));
			}

			template<typename T>
			const T& GetArgument(const std::string& a_sName)
			{
				for (const BaseArg* arg : m_aArgs)
				{
					if (arg->GetName() == a_sName)
					{
						const Arg<T>* cArg = dynamic_cast<const Arg<T>*>(arg);
						if (cArg != nullptr)
						{
							return cArg->GetValue();
						}
					}
				}
			}
		private:
			std::vector<BaseArg*> m_aArgs;
		};
		inline extern ArgProcessor ARGS = {};
	}
}