#pragma once
#include <string>
#include <locale>
#include <codecvt>

namespace Lambda
{
	namespace StringUtilities
	{
		/*///////////////////////////////////////////////////////////////////*/
		_forceinline std::wstring ConvertFromString(const std::string& string)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			return converter.from_bytes(string);
		}

		/*///////////////////////////////////////////////////////////////////*/
		_forceinline std::string ConvertFromWidestring(const std::wstring& string)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			return converter.to_bytes(string);
		}
	}
}