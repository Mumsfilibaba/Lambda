#include "LambdaPch.h"

#include "Core/Platform.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsPlatform.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/MacPlatform.h"
#endif

namespace Lambda
{
	//--------
	//Platform
	//--------

    namespace Platform
    {
        /*/////////////////////////////////////////////////////////////////////////////////////*/
        void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            WindowsPlatform::MessageBox(pCaption, pText, type);
#elif defined(LAMBDA_PLAT_MACOS)
            MacPlatform::MessageBox(pCaption, pText, type);
#endif
        }

        /*//////////////////////////////*/
        uint64 QueryPerformanceFrequency()
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            return WindowsPlatform::QueryPerformanceFrequency();
#elif defined(LAMBDA_PLAT_MACOS)
            return MacPlatform::QueryPerformanceFrequency();
#endif
        }
    
        /*////////////////////////////*/
        uint64 QueryPerformanceCounter()
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            return WindowsPlatform::QueryPerformanceCounter();
#elif defined(LAMBDA_PLAT_MACOS)
            return MacPlatform::QueryPerformanceCounter();
#endif
        }

    }
}
