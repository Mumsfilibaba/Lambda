#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsTime.h"

namespace Lambda
{
	namespace Windows
	{
		namespace Time
		{
			LARGE_INTEGER g_Frequency = { 0 };

			void Init()
			{
				::QueryPerformanceFrequency(&g_Frequency);
			}

			uint64 Nanoseconds()
			{
				LARGE_INTEGER counter = {};
				::QueryPerformanceCounter(&counter);

				counter.QuadPart *= 1000000000UL;
				return uint64(counter.QuadPart / g_Frequency.QuadPart);
			}
		}
	}
}
#endif