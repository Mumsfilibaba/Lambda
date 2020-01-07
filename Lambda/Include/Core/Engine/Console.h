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
		friend class CConsole;
	public:
		LAMBDA_INTERFACE(IConsole);

		virtual void SetTitle(const char* pTitle) = 0;
		virtual void SetTextColor(EConsoleColor color) = 0;

		void Print(const char* pFormat, ...)
        {
            va_list args;
            va_start(args, pFormat);
            Print(pFormat, args);
            va_end(args);
        }
		virtual void Print(const char* pFormat, va_list args) = 0;
		
		void PrintLine(const char* pFormat, ...)
        {
            va_list args;
            va_start(args, pFormat);
            PrintLine(pFormat, args);
            va_end(args);
        }
		virtual void PrintLine(const char* pFormat, va_list args) = 0;

		virtual void Reset() = 0;
		virtual void Clear() = 0;
		
		virtual void Release() = 0;
	private:
		virtual bool Initialize() = 0;
	};

	//--------
	//CConsole
	//--------
	class LAMBDA_API CConsole
	{
	public:
		LAMBDA_STATIC_CLASS(CConsole);

		static bool Initialize();
		static void Release();

		/*////////////////////////////////////*/
		static void SetTitle(const char* pTitle)
		{
			s_pConsole->SetTitle(pTitle);
		}

		/*/////////////////////////////////////////*/
		static void SetTextColor(EConsoleColor color)
		{
			s_pConsole->SetTextColor(color);
		}

		/*///////////////////////////////////////*/
		static void Print(const char* pFormat, ...)
		{
			va_list args;
			va_start(args, pFormat);
			s_pConsole->Print(pFormat, args);
			va_end(args);
		}

		/*////////////////////////////////////////////////*/
		static void Print(const char* pFormat, va_list args)
		{
			s_pConsole->Print(pFormat, args);
		}

		/*///////////////////////////////////////////*/
		static void PrintLine(const char* pFormat, ...)
		{
			va_list args;
			va_start(args, pFormat);
			s_pConsole->PrintLine(pFormat, args);
			va_end(args);
		}

		/*////////////////////////////////////////////////////*/
		static void PrintLine(const char* pFormat, va_list args)
		{
			s_pConsole->PrintLine(pFormat, args);
		}

		/*///////////////*/
		static void Reset()
		{
			s_pConsole->Reset();
		}

		/*///////////////*/
		static void Clear()
		{
			s_pConsole->Clear();
		}

		/*//////////////////*/
		static IConsole* Get()
		{
			return s_pConsole;
		}
	private:
		static IConsole* s_pConsole;
	};
}