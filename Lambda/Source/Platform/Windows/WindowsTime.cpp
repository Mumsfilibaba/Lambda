#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsTime.h"

namespace Lambda
{
	LARGE_INTEGER CWindowsTime::s_Frequency = { 0 };
	
	void CWindowsTime::Init()
	{
		::QueryPerformanceFrequency(&s_Frequency);
	}

	uint64 CWindowsTime::Nanoseconds()
	{
		LARGE_INTEGER counter = {};
		::QueryPerformanceCounter(&counter);

		counter.QuadPart *= 1000000000UL;
		return uint64(counter.QuadPart / s_Frequency.QuadPart);
	}
	
	uint64 CWindowsTime::Ticks()
	{
		LARGE_INTEGER counter = {};
		::QueryPerformanceCounter(&counter);
		return uint64(counter.QuadPart / s_Frequency.QuadPart);
	}
	
	uint64 CWindowsTime::TicksPerSecond()
	{
		return uint64(s_Frequency.QuadPart);
	}
}
#endif