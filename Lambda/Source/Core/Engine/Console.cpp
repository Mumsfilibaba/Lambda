#include "LambdaPch.h"

#include "Core/Engine/Console.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsConsole.h"
#elif defined(LAMBDA_PLAT_MACOS)
	#include "Platform/macOS/MacConsole.h"
#endif

namespace Lambda
{
	//--------
	//CConsole
	//--------

	IConsole* CConsole::s_pConsole = nullptr;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CConsole::Initialize()
	{
		LAMBDA_ASSERT_PRINT(s_pConsole == nullptr, "CConsole::Initialize should only be called once");

		//Create console interface
#if defined(LAMBDA_PLAT_WINDOWS)
		s_pConsole = CWindowsConsole::CreateWindowsConsole();
#elif defined(LAMBDA_PLAT_MACOS)
		s_pConsole = DBG_NEW CMacConsole();
#endif
	}

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CConsole::Release()
	{
		LAMBDA_ASSERT_PRINT(s_pConsole != nullptr, "CConsole is not initialized");
		
		//Release and set to nullptr, this means that initialize can safley be called after a call to release
		s_pConsole->Release();
		s_pConsole = nullptr;
	}
}
