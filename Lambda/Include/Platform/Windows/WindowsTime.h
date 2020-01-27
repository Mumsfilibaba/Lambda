#pragma once
#include "Platform/Common/CommonTime.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN 
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

namespace Lambda
{
	class LAMBDA_API CWindowsTime final : public CCommonTime
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CWindowsTime);

		_forceinline static void Init()
		{
			::QueryPerformanceFrequency(&s_Frequency);
		}

		_forceinline static uint64 Nanoseconds()
		{
			LARGE_INTEGER counter = {};
			::QueryPerformanceCounter(&counter);

			return uint64((counter.QuadPart * 1000000000UL) / s_Frequency.QuadPart);
		}

		_forceinline static uint64 Ticks()
		{
			LARGE_INTEGER counter = {};
			::QueryPerformanceCounter(&counter);

			return uint64(counter.QuadPart / s_Frequency.QuadPart);
		}

		_forceinline static uint64 TicksPerSecond()
		{
			return uint64(s_Frequency.QuadPart);
		}
	private:
		inline static LARGE_INTEGER s_Frequency = { 0 };
	};

	typedef CWindowsTime PlatformTime;
}
#endif