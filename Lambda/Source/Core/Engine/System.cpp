#include "LambdaPch.h"

#include "Core/Engine/System.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsSystem.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/MacSystem.h"
#endif

namespace Lambda
{
	//-------
	//ISystem
	//-------

	ISystem* ISystem::Create(IEngine* pEngine)
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		return DBG_NEW CWindowsSystem(pEngine);
#elif defined(LAMBDA_PLAT_MACOS)
        return DBG_NEW CMacSystem(pEngine);
#else
	#error No platform defined
		return nullptr;
#endif
	}
}
