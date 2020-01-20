#pragma once
#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN 
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

namespace Lambda
{
	namespace Windows
	{
		namespace Application
		{
			LAMBDA_API void Init(HINSTANCE hInstance);
			LAMBDA_API void PollMessages();

			LAMBDA_API HINSTANCE GetApplicationInstance();
		}
	}

	namespace Platform = Windows;
}
#endif