#include "ArgProcessor.h"

namespace gallus
{
	namespace core
	{
		BaseArg::BaseArg(const std::string& a_sName) : m_sName(a_sName)
		{}

        bool hasCommandArg(const std::string& args, const std::string& argName)
        {
            std::string search = argName + "=";
            return args.find(search) != std::string::npos;
        }

        std::string getCommandArg(const std::string& args, const std::string& argName, const std::string& defaultVal)
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

		std::vector<BaseArg*> ArgProcessor::GetArguments(const std::string& a_sArgs)
		{
            std::vector<BaseArg*> args;
            if (hasCommandArg(a_sArgs, ASSET_PATH_ARG))
            {
                std::string assetPath = getCommandArg(a_sArgs, ASSET_PATH_ARG, "./data/assets/");
                args.push_back(new Arg<std::string>(ASSET_PATH_ARG, assetPath));
            }
            return args;
		}
	}
}