#pragma once

// external
#include <string>

namespace gallus
{
	namespace string_extensions
	{
		/// <summary>
		/// Converts a string to upper case.
		/// </summary>
		/// <param name="a_sStrToConvert">The string that will be converted.</param>
		/// <returns>A string containing the input string in upper case.</returns>
		std::string StringToUpper(std::string a_sStrToConvert);

		/// <summary>
		/// Converts a string to lower case.
		/// </summary>
		/// <param name="a_sStrToConvert">The string that will be converted.</param>
		/// <returns>A string containing the input string in lower case.</returns>
		std::string StringToLower(std::string a_sStrToConvert);
	}
}