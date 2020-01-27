#pragma once
#include "Core/Debug/IConsoleOutput.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#define MAX_OUTPUT_CHARS 1024

	#ifndef WIN32_LEAN_AND_MEAN 
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

namespace Lambda
{
	class CWindowsConsoleOutput final : public IConsoleOutput
	{
	public:
		CWindowsConsoleOutput();
		~CWindowsConsoleOutput();

		virtual void Show() override final;
		virtual void Hide() override final;

		virtual void SetTitle(const char* pTitle) override final;
		virtual void SetTextColor(EConsoleColor color) override final;

		virtual void Write(const char* pFormat, ...) override final;
		virtual void WriteV(const char* pFormat, va_list args) override final;

		virtual void Clear() override final;
	private:
		HANDLE m_hOutput;
		EConsoleColor m_CurrentColor;
	};
}
#endif