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
        //----
        //Misc
        //----

        namespace Misc
        {
            /*////////////////////////////////////////////////////////////////////////////////////////////////*/
            void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
            {
#if defined(LAMBDA_PLAT_WINDOWS)
                WindowsPlatform::Misc::MessageBox(pCaption, pText, type);
#elif defined(LAMBDA_PLAT_MACOS)
                MacPlatform::MessageBox(pCaption, pText, type);
#endif
            }
        }

        //----
        //Time
        //----

        namespace Time
        {
            /*////////////////////////////////////////////////////////////////////////////////////////////////*/
            uint64 QueryPerformanceFrequency()
            {
#if defined(LAMBDA_PLAT_WINDOWS)
                return WindowsPlatform::Time::QueryPerformanceFrequency();
#elif defined(LAMBDA_PLAT_MACOS)
                return MacPlatform::QueryPerformanceFrequency();
#endif
            }
    
            /*////////////////////////////////////////////////////////////////////////////////////////////////*/
            uint64 QueryPerformanceCounter()
            {
#if defined(LAMBDA_PLAT_WINDOWS)
                return WindowsPlatform::Time::QueryPerformanceCounter();
#elif defined(LAMBDA_PLAT_MACOS)
                return MacPlatform::QueryPerformanceCounter();
#endif
            }
        }
    }
}
