#pragma once

// external
#include <string>
#include <vector>

constexpr const char* ASSET_PATH_ARG = "assetPath";
constexpr const char* ASSERT_LEVEL_ARG = "assertLevel";
constexpr const char* LOG_TO_FILE_ARG = "logToFile";
constexpr const char* LOG_TYPE_ARG = "logType";

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// BaseArg
		//---------------------------------------------------------------------
		/// <summary>
		/// Generic class used for arguments. Pure virtual.
		/// </summary>
		class BaseArg
		{
		public:
			/// <summary>
			/// Constructs a BaseArg.
			/// </summary>
			/// <param name="a_sName">The name of the argument.</param>
			BaseArg(const std::string& a_sName) : m_sName(a_sName) {}

			/// <summary>
			/// Destructor for BaseArg.
			/// </summary>
			virtual ~BaseArg() = default;

			/// <summary>
			/// Retrieves the name of the argument.
			/// </summary>
			/// <returns>Reference to the string containing the name of the argument.</returns>
			virtual const std::string& GetName() const { return m_sName; }

			/// <summary>
			/// Retrieves the value as a string.
			/// </summary>
			/// <returns>The argument value returned as string.</returns>
			virtual std::string GetValueAsString() const = 0;

			/// <summary>
			/// Sets the value by string.
			/// </summary>
			/// <param name="a_sValueStr">The value of the argument.</param>
			virtual void SetValueFromString(const std::string& a_sValueStr) = 0;
		protected:
			std::string m_sName;
		};

		//---------------------------------------------------------------------
		// Arg
		//---------------------------------------------------------------------
		/// <summary>
		/// Class used for arguments. Generic, type can be changed.
		/// </summary>
		template<typename T>
		class Arg : public BaseArg
		{
		public:
			/// <summary>
			/// Constructs an Arg with given value.
			/// </summary>
			/// <param name="a_sName">The name of the argument.</param>
			/// <param name="a_Value">The value of the argument.</param>
			Arg(const std::string& a_sName, const T& a_Value)
				: BaseArg(a_sName), m_Value(a_Value)
			{}

			/// <summary>
			/// Retrieves the value.
			/// </summary>
			/// <returns>The argument value.</returns>
			const T& GetValue() const { return m_Value; }

			/// <summary>
			/// Sets the value
			/// </summary>
			/// <param name="a_Value">The value of the argument.</param>
			void SetValue(const T& a_Value) { m_Value = a_Value; }

			/// <summary>
			/// Retrieves the value as a string.
			/// </summary>
			/// <returns>The argument value returned as string.</returns>
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

			/// <summary>
			/// Sets the value by string.
			/// </summary>
			/// <param name="a_sValueStr">The value of the argument.</param>
			void SetValueFromString(const std::string& a_sValueStr) override
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					m_Value = a_sValueStr;
				}
				else if constexpr (std::is_same_v<T, bool>)
				{
					m_Value = (a_sValueStr == "1" || a_sValueStr == "true");
				}
				else
				{
					m_Value = static_cast<T>(std::stoi(a_sValueStr));
				}
			}
		private:
			T m_Value;
		};

		//---------------------------------------------------------------------
		// ArgProcessor
		//---------------------------------------------------------------------
		/// <summary>
		/// This class processes all arguments and puts them in an array.
		/// </summary>
		class ArgProcessor
		{
		public:
			/// <summary>
			/// Processes the arguments.
			/// </summary>
			/// <param name="a_sArgs">The string containing all arguments from cmd.</param>
			void ProcessArguments(const std::string& a_sArgs);

			/// <summary>
			/// Adds an argument with a default value.
			/// </summary>
			/// <param name="a_sName">The name of the argument.</param>
			/// <param name="a_Value">The value of the argument.</param>
			template<typename T>
			void AddArgument(const std::string& a_sName, const T& a_Value)
			{
				m_aArgs.push_back(new Arg<T>(a_sName, a_Value));
			}

			/// <summary>
			/// Retrieves the value of a given argument.
			/// </summary>
			/// <param name="a_sName">The name of the argument.</param>
			/// <returns>The argument value.</returns>
			template<typename T>
			const T GetArgument(const std::string& a_sName)
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

				return T();
			}
		private:
			std::vector<BaseArg*> m_aArgs;
		};
		inline extern ArgProcessor ARGS = {};
	}
}