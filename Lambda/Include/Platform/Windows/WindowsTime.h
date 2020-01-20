#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_WINDOWS

namespace Lambda
{
	namespace Windows
	{
		namespace Time
		{
			LAMBDA_API void Init();
			LAMBDA_API uint64 Nanoseconds();
		}
	}

	namespace Platform = Windows;
}
#endif