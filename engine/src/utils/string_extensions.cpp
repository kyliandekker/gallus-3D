#include "string_extensions.h"

// external
#include <algorithm>

namespace gallus
{
	namespace string_extensions
	{
		//---------------------------------------------------------------------
		std::string StringToUpper(std::string a_sStrToConvert)
		{
			std::transform(a_sStrToConvert.begin(), a_sStrToConvert.end(), a_sStrToConvert.begin(), ::toupper);

			return a_sStrToConvert;
		}

		//---------------------------------------------------------------------
		std::string StringToLower(std::string a_sStrToConvert)
		{
			std::transform(a_sStrToConvert.begin(), a_sStrToConvert.end(), a_sStrToConvert.begin(), ::tolower);

			return a_sStrToConvert;
		}

		//---------------------------------------------------------------------
		std::string SeparateByCapitals(std::string a_sStrToConvert)
		{
			std::string tempStr;
			tempStr.reserve(a_sStrToConvert.size() * 2);

			for (std::size_t i = 0; i < a_sStrToConvert.size(); ++i)
			{
				char c = a_sStrToConvert[i];

				if (i > 0 &&
					std::isupper(static_cast<unsigned char>(c)) &&
					a_sStrToConvert[i - 1] != ' ' &&
					a_sStrToConvert[i - 1] != '_')
				{
					tempStr.push_back(' ');
				}

				tempStr.push_back(c);
			}

			return tempStr;
		}
		
		//---------------------------------------------------------------------
		std::string LowerCamelCasify(std::string a_sStrToConvert)
		{
			a_sStrToConvert[0] = ::tolower(a_sStrToConvert[0]);
			return a_sStrToConvert;
		}
	}
}