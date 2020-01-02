#pragma once
#include <string>

namespace Lambda
{
	///////////////////////////////////////////////////////////////////////
	_forceinline std::wstring StringToWidestring(const std::string& string)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.from_bytes(string);
	}

	///////////////////////////////////////////////////////////////////////
	_forceinline std::string WidestringToString(const std::wstring& string)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(string);
	}
}