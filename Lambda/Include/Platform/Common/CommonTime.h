#pragma once
#include "LambdaCore.h"

namespace Lambda
{
	class CCommonTime
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CCommonTime);

		static uint64 NanoSeconds()		{ return 0; }
		static uint64 Ticks()			{ return 0; }
		static uint64 TicksPerSecond()	{ return 0; }
	};
}