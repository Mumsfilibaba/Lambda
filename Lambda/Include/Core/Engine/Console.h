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

	class IConsole
	{
	public:
		LAMBDA_INTERFACE(IConsole);

		virtual void SetTitle(const char* pTitle) = 0;
		virtual void SetTextColor(EConsoleColor color) = 0;

		virtual void Print(const char* pFormat, ...) = 0;
		virtual void PrintV(const char* pFormat, va_list args) = 0;
		
		virtual void PrintLine(const char* pFormat, ...) = 0;
		virtual void PrintLineV(const char* pFormat, va_list args) = 0;

		virtual void Reset() = 0;
		virtual void Clear() = 0;
	};

	//-------
	//Console
	//-------

	class LAMBDA_API Console
	{
	public:
		LAMBDA_STATIC_CLASS(Console);

		static void Initialize();
		static void Release();

		inline static void SetTitle(const char* pTitle)		 { s_pConsole->SetTitle(pTitle); }
		inline static void SetTextColor(EConsoleColor color) { s_pConsole->SetTextColor(color); }

		inline static void Print(const char* pFormat, va_list args)		{ s_pConsole->PrintV(pFormat, args); }
		inline static void PrintLine(const char* pFormat, va_list args) { s_pConsole->PrintLineV(pFormat, args); }

		inline static void Reset() { s_pConsole->Reset(); }
		inline static void Clear() { s_pConsole->Clear(); }

		inline static void Print(const char* pFormat, ...)
		{
			va_list args;
			va_start(args, pFormat);
			s_pConsole->PrintV(pFormat, args);
			va_end(args);
		}

		inline static void PrintLine(const char* pFormat, ...)
		{
			va_list args;
			va_start(args, pFormat);
			s_pConsole->PrintLineV(pFormat, args);
			va_end(args);
		}
	private:
		static IConsole* s_pConsole;
	};
}
