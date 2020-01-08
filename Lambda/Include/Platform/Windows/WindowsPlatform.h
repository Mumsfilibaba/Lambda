#pragma once
#include "Core/Platform.h"
#if defined(LAMBDA_PLAT_WINDOWS)

namespace Lambda
{
	//---------------
	//WindowsPlatform
	//---------------
	namespace WindowsPlatform
	{
		//----
		//Misc
		//----
		namespace Misc
		{
			void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);
		}

		//----
		//Time
		//----
		namespace Time
		{
			uint64 QueryPerformanceCounter();
			uint64 QueryPerformanceFrequency();
		}
	}
}
#endif