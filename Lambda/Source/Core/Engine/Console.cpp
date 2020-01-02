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

	/*///////////////////////*/
	bool CConsole::Initialize()
	{
		LAMBDA_ASSERT_PRINT(s_pConsole == nullptr, "CConsole::Initialize should only be called once");

		//Create console interface
		IConsole* pConsole = nullptr;
#if defined(LAMBDA_PLAT_WINDOWS)
		pConsole = DBG_NEW CWindowsConsole();
#elif defined(LAMBDA_PLAT_MACOS)
		pConsole = DBG_NEW CmacOSConsole();
#endif

		//Initialize console
		s_pConsole = pConsole;
		if (s_pConsole)
		{
			return s_pConsole->Initialize();
		}

		return false;
	}

    /*////////////////////*/
	void CConsole::Release()
	{
		LAMBDA_ASSERT_PRINT(s_pConsole != nullptr, "CConsole is not initialized");
		
		//Release and set to nullptr, this means that initialize can safley be called after a call to release
		s_pConsole->Release();
		s_pConsole = nullptr;
	}
}
