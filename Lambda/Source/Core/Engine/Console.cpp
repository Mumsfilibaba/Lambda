#include "LambdaPch.h"

#include "Core/Engine/Console.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsConsole.h"
#elif defined(LAMBDA_PLAT_MACOS)
	#include "Platform/macOS/MacConsole.h"
#endif

namespace Lambda
{
	//-------
	//Console
	//-------

	IConsole* Console::s_pInstance = nullptr;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Console::Initialize()
	{
		LAMBDA_ASSERT_PRINT(s_pInstance == nullptr, "Console::Initialize should only be called once");

		//Create console interface
#if defined(LAMBDA_PLAT_WINDOWS)
		s_pInstance = WindowsConsole::CreateWindowsConsole();
#elif defined(LAMBDA_PLAT_MACOS)
		s_pInstance = DBG_NEW MacConsole();
#endif
	}

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Console::Release()
	{
		LAMBDA_ASSERT_PRINT(s_pInstance != nullptr, "Console is not initialized");
		
		//Release and set to nullptr, this means that initialize can safley be called after a call to release
		SafeDelete(s_pInstance);
	}
}
