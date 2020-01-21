#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsApplication.h"
	#include "Platform/Windows/WindowsTime.h"

namespace Lambda
{
	HINSTANCE CWindowsApplication::s_hInstance = 0;

	void CWindowsApplication::Init(HINSTANCE hInstance)
	{
		//Activate debug memory check
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		//Set global instance
		s_hInstance = hInstance;

		//Init other modules
		CWindowsTime::Init();
	}

	void CWindowsApplication::PollEvents()
	{
		MSG message = {};
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
}
#endif