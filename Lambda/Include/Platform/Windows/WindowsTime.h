#pragma once
#include "LambdaCore.h"

#include "Platform/Generic/GenericTime.h"

#ifdef LAMBDA_PLAT_WINDOWS

namespace Lambda
{
	class LAMBDA_API CWindowsTime : public CGenericTime
	{
	public:
		LAMBDA_STATIC_CLASS(CWindowsTime);

		static void Init();
		static uint64 Nanoseconds();
		static uint64 Ticks();
		static uint64 TicksPerSecond();
	private:
		static LARGE_INTEGER s_Frequency;
	};

	typedef CWindowsTime PlatformTime;
}
#endif