#pragma once
#include "Core/Engine/Console.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#define MAX_OUTPUT_CHARS 1024

namespace Lambda
{
	//---------------
	//CWindowsConsole
	//---------------
	class CWindowsConsole final : public IConsole
	{
	public:
		virtual void SetTitle(const char* pTitle) override final;
		virtual void SetTextColor(EConsoleColor color) override final;

		virtual void Print(const char* pFormat, ...) override final;
		virtual void PrintV(const char* pFormat, va_list args) override final;

		virtual void PrintLine(const char* pFormat, ...) override final;
		virtual void PrintLineV(const char* pFormat, va_list args) override final;

		virtual void Reset() override final;
		virtual void Clear() override final;

		virtual void Release() override final;
	private:
		CWindowsConsole();
		~CWindowsConsole();
	private:
		HANDLE m_hOutput;
		EConsoleColor m_CurrentColor;
	public:
		static IConsole* CreateWindowsConsole();
	};
}
#endif