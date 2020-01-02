#include "LambdaPch.h"
#include "Platform/Windows/WindowsPlatform.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#define WIN32_LEAN_AND_MEAN 1	
	#include <Windows.h>

namespace Lambda
{
	//---------------
	//WindowsPlatform
	//---------------

	namespace WindowsPlatform
	{
		/*//////////////////////////////////////////////////////////////////////////*/
		void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
		{
			UINT uType = 0;
			if (type == EMessageBoxType::MESSAGE_BOX_TYPE_ERROR)
			{
				uType = MB_OK | MB_ICONERROR;
			}

			::MessageBoxA(0, pText, pCaption, uType);
		}

        /*//////////////////////////////*/
        uint64 QueryPerformanceFrequency()
        {
            LARGE_INTEGER frequency = {};
            ::QueryPerformanceFrequency(&frequency);
            
            return uint64(frequency.QuadPart);
        }

        /*////////////////////////////*/
        uint64 QueryPerformanceCounter()
        {
            LARGE_INTEGER counter = {};
            ::QueryPerformanceCounter(&counter);

            return 1000000000UL * uint64(counter.QuadPart);
        }

	}
}
#endif
