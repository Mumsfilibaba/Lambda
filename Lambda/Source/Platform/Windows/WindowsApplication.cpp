#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsApplication.h"
	#include "Platform/Windows/WindowsTime.h"

namespace Lambda
{
	namespace Windows
	{
		namespace Application
		{
			HINSTANCE g_hInstance = 0;

			void Init(HINSTANCE hInstance)
			{
				//Set global instance
				g_hInstance = hInstance;
				
				//Init other modules
				Time::Init();
			}

			void PollMessages()
			{
				MSG message = {};
				while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
			}

			HINSTANCE GetApplicationInstance()
			{
				return g_hInstance;
			}
		}
	}
}
#endif