#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class LAMBDA_API CGenericTime
	{
	public:
		LAMBDA_STATIC_CLASS(CGenericTime);

		static uint64 NanoSeconds() { return 0; }
		static uint64 Ticks() { return 0; }
		static uint64 TicksPerSecond() { return 0; }
	};
}
