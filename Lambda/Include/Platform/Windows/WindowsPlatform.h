#pragma once
#include "Core/Platform.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	//----------------
	//CWindowsPlatform
	//----------------
	class CWindowsPlatform
	{
	public:
		LAMBDA_STATIC_CLASS(CWindowsPlatform);

		static void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);

		static uint64 QueryPerformanceCounter();
		static uint64 QueryPerformanceFrequency();
	};
}
#endif