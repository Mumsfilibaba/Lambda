#pragma once
#include "CPlatform.h"
#include <stdarg.h>

namespace Lambda
{
	//--------
	//CConsole
	//--------

	class CConsole
	{
	public:
		LAMBDA_STATIC_CLASS(CConsole);

		static bool Initialize()
		{
			return CPlatform::InitializeConsole();
		}

		static void SetTextColor(EConsoleColor color)
		{
			CPlatform::SetConsoleTextColor(color);
		}

		static void Print(const char* pFormat, ...)
		{
			va_list args;
			va_start(args, pFormat);
			CPlatform::ConsolePrint(pFormat, args);
			va_end(args);
		}

		static void Release()
		{
			CPlatform::ReleaseConsole();
		}
	};
}