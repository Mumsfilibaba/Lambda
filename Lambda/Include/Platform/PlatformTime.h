#pragma once
#ifdef LAMBDA_PLAT_WINDOWS
	#include "Windows/WindowsTime.h"
#elif defined(LAMBDA_PLAT_MACOS)
	#include "macOS/MacTime.h"
#endif
