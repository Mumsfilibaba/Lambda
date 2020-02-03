#pragma once
#ifdef LAMBDA_PLAT_WINDOWS
	#include "Windows/WindowsInput.h"
#elif defined(LAMBDA_PLAT_MACOS)
	#include "macOS/MacInput.h"
#endif
