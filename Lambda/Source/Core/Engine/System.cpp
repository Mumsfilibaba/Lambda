#include "LambdaPch.h"

#include "Core/Engine/System.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsSystem.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/MacSystem.h"
#endif

namespace Lambda
{
	//------
	//System
	//------

	ISystem* System::s_pInstance = nullptr;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void System::Initialize()
	{
		LAMBDA_ASSERT_PRINT(s_pInstance == nullptr, "Application can only be initialized once");

#if defined(LAMBDA_PLAT_WINDOWS)
		s_pInstance = WindowsSystem::Create();
#elif defined(LAMBDA_PLAT_MACOS)
		s_pInstance = DBG_NEW MacSystem(pEngine);
#else
	#error No platform defined
#endif
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void System::Release()
	{
		LAMBDA_ASSERT_PRINT(s_pInstance != nullptr, "Application not initialized");
		SafeDelete(s_pInstance);
	}
}
