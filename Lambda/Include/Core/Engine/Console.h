#pragma once
#include "LambdaCore.h"

#include <stdarg.h>

namespace Lambda
{
	//-------------
	//EConsoleColor
	//-------------
	enum class EConsoleColor : uint32
	{
		CONSOLE_COLOR_UNKNOWN	= 0,
		CONSOLE_COLOR_WHITE		= 1,
		CONSOLE_COLOR_RED		= 2,
		CONSOLE_COLOR_GREEN		= 3,
		CONSOLE_COLOR_YELLOW	= 4,
	};

	//--------
	//IConsole
	//--------
	class LAMBDA_API IConsole
	{
	public:
		LAMBDA_INTERFACE(IConsole);

		virtual void SetTitle(const char* pTitle) = 0;
		virtual void SetTextColor(EConsoleColor color) = 0;

		virtual void Print(const char* pFormat, ...) = 0;
		virtual void Print(const char* pFormat, va_list args) = 0;
		
		virtual void PrintLine(const char* pFormat, ...) = 0;
		virtual void PrintLine(const char* pFormat, va_list args) = 0;

		virtual void Reset() = 0;
		virtual void Clear() = 0;
		
		virtual void Release() = 0;
	public:
		static IConsole* Create();
	};
}