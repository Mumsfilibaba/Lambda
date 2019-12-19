#pragma once
#include "Core/CEngine.h"

extern Lambda::CLayer* Lambda::CreateGameLayer(void);

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Source/Platform/Windows/WindowsEntryPoint.h"
#elif defined(LAMBDA_PLAT_MACOS)
	#include "../Source/Platform/macOS/macOSEntryPoint.h"
#endif
