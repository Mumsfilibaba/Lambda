#pragma once
#include "Defines.h"
#include "Types.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>
	#ifdef MessageBox
		#undef MessageBox
	#endif

namespace Lambda
{
	//----------------
	//CWindowsPlatform
	//----------------

	class CWindowsPlatform
	{
	public:
		LAMBDA_STATIC_CLASS(CWindowsPlatform);

		static bool Initialize();
		static void ProcessEvents();

		static void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);

		static void InitializeKeycodeLookupTable();

		static bool InitializeConsole();
		static void SetConsoleTextColor(EConsoleColor color);
		static void ConsolePrint(const char* pFormat, va_list args);
		static void ReleaseConsole();

		static void ShowCursor(bool bVisible);
		static void SetCursorPosition(int32 x, int32 y);
		static void GetCursorPosition(int32& outX, int32& outY);

		static LRESULT MessageProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
}
#endif