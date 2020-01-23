#pragma once
#ifdef LAMBDA_PLAT_WINDOWS
	#include "Windows/WindowsApplication.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "macOS/MacApplication.h"
#endif
