#include "ArgProcessor.h"

namespace gallus::core
{
	//---------------------------------------------------------------------
	bool hasCommandArg(const std::string& args, const std::string& argName)
	{
		std::string search = argName + "=";
		return args.find(search) != std::string::npos;
	}

	//---------------------------------------------------------------------
	std::string getCommandArgStr(const std::string& args, const std::string& argName, const std::string& defaultVal)
	{
		if (!hasCommandArg(args, argName))
		{
			return defaultVal;
		}

		std::string search = argName + "=";
		size_t pos = args.find(search);
		size_t start = pos + search.size();

		if (start >= args.size())
		{
			return defaultVal;
		}

		// Quoted value
		if (args[start] == '"')
		{
			size_t end = args.find('"', start + 1);
			if (end != std::string::npos)
			{
				return args.substr(start + 1, end - start - 1);
			}
			else
			{
				return defaultVal; // no closing quote
			}
		}
		else
		{
			// Unquoted value
			size_t end = args.find(' ', start);
			if (end == std::string::npos)
			{
				return args.substr(start);
			}
			else
			{
				return args.substr(start, end - start);
			}
		}
	}

	//---------------------------------------------------------------------
	int getCommandArgInt(const std::string& args, const std::string& argName, const int& defaultVal)
	{
		return std::stoi(getCommandArgStr(args, argName, std::to_string(defaultVal)));
	}

	//---------------------------------------------------------------------
	void ArgProcessor::ProcessArguments(const std::string& a_sArgs)
	{
		for (std::unique_ptr<BaseArg>& arg : m_aArgs)
		{
			if (hasCommandArg(a_sArgs, arg->GetName()))
			{
				std::string value = getCommandArgStr(a_sArgs, arg->GetName(), arg->GetValueAsString());
				arg->SetValueFromString(value);
			}
		}
	}
}