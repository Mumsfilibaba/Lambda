#pragma once
#ifdef LAMBDA_PLAT_WINDOWS
	#include "Windows/WindowsPlatform.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "macOS/MacPlatform.h"
#endif
