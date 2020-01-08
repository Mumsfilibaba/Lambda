#include "LambdaPch.h"

#include "Core/Engine/Application.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsApplication.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/MacSystem.h"
#endif

namespace Lambda
{
	//------------
	//CApplication
	//------------

	IApplication* CApplication::s_pApplication = nullptr;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CApplication::Initialize()
	{
		LAMBDA_ASSERT_PRINT(s_pApplication == nullptr, "Application can only be initialized once");

#if defined(LAMBDA_PLAT_WINDOWS)
		s_pApplication = CWindowsApplication::CreateWindowsApplication();
#elif defined(LAMBDA_PLAT_MACOS)
		s_pApplication = DBG_NEW CMacSystem(pEngine);
#else
	#error No platform defined
#endif
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CApplication::Release()
	{
		LAMBDA_ASSERT_PRINT(s_pApplication != nullptr, "Application not initialized");

		s_pApplication->Release();
		s_pApplication = nullptr;
	}
}
