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
	namespace WindowsPlatform
	{
		void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);

		void InitializeKeycodeLookupTable();

		void ShowCursor(bool bVisible);
		void SetCursorPosition(int32 x, int32 y);
		void GetCursorPosition(int32& outX, int32& outY);
	}
}
#endif