#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
#include "CWindowsKeyboardController.h"
#include "CWindowsEnvironment.h"
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

namespace Lambda
{
	//-------------------------
	//CWindowKeyboardController
	//-------------------------

	CWindowsKeyboardController::CWindowsKeyboardController()
	{
	}


	bool CWindowsKeyboardController::IsKeyUp(EKey key) const
	{
		CWindowsEnvironment& environment = GET_WINDOWS_ENVIRONMENT();
		return !(GetAsyncKeyState(environment.ConvertKeyFromEngine(key)) & 0x8000);
	}


	bool CWindowsKeyboardController::IsKeyDown(EKey key) const
	{
		CWindowsEnvironment& environment = GET_WINDOWS_ENVIRONMENT();
		return GetAsyncKeyState(environment.ConvertKeyFromEngine(key)) & 0x8000;
	}


	CKeyboardState CWindowsKeyboardController::GetKeyboardState() const
	{
		CKeyboardState keyboardState;

		BYTE keys[256];
		GetKeyState(0);
		if (::GetKeyboardState(keys))
		{
			CWindowsEnvironment& environment = GET_WINDOWS_ENVIRONMENT();

			//Loop all keys and set state
			EKey key = KEY_UNKNOWN;
			for (uint32 i = 0; i < 256; i++)
			{
				key = environment.ConvertKeyFromWindows(i);
				if (keys[i] & 0x80)
					keyboardState.SetKeyStateDown(key);
				else
					keyboardState.SetKeyStateUp(key);
			}
		}

		return keyboardState;
	}
}
#endif