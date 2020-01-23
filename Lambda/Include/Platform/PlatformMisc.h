#pragma once
#ifdef LAMBDA_PLAT_WINDOWS
	#include "Windows/WindowsMisc.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "macOS/MacMisc.h"
#endif
