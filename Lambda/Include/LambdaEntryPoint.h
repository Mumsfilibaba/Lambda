#pragma once
#include "Core/CEngine.h"

extern Lambda::CLayer* Lambda::CreateGameLayer(void);

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsEntryPoint.h"
#elif defined(LAMBDA_PLAT_MACOS)
	#include "Platform/macOS/macOSEntryPoint.h"
#endif
