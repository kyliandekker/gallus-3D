#pragma once

// external
#include <string>

namespace gallus::string_extensions
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

	/// <summary>
	/// Separates a string by capital letters.
	/// </summary>
	/// <param name="a_sStrToConvert">The string that will be converted.</param>
	/// <returns>A string with spaces separating the capital letters.</returns>
	std::string SeparateByCapitals(std::string a_sStrToConvert);

	/// <summary>
	/// Makes a string lower camel case.
	/// </summary>
	/// <param name="a_sStrToConvert">The string that will be converted.</param>
	/// <returns>A string with spaces separating the capital letters.</returns>
	std::string LowerCamelCasify(std::string a_sStrToConvert);
}