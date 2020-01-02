#include "LambdaPch.h"

#include "Core/Engine/System.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsSystem.h"
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
#else
	#error No platform defined
		return nullptr;
#endif
	}
}