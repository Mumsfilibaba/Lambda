#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class CommonTime
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CommonTime);

		static uint64 NanoSeconds()		{ return 0; }
		static uint64 Ticks()			{ return 0; }
		static uint64 TicksPerSecond()	{ return 0; }
	};
}