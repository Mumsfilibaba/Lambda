#include "LambdaPch.h"

#include "Core/Engine/System.h"
#include "Core/Log/LogManager.h"

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

		//Create log for system
		LogManager::Get().CreateLog("System", ELogMode::LOG_MODE_TRUNCATE, ELogVerbosity::LOG_VERBOSITY_ERROR, true, false);

		//Create instance
#if defined(LAMBDA_PLAT_WINDOWS)
		s_pInstance = WindowsSystem::Create();
#elif defined(LAMBDA_PLAT_MACOS)
		s_pInstance = MacSystem::Create();
#else
	#error No platform defined
#endif
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void System::Detach()
	{
		LAMBDA_ASSERT_PRINT(s_pInstance != nullptr, "Application not initialized");
		SafeDelete(s_pInstance);
	}
}
