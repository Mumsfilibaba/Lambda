#include "LambdaPch.h"
#include "Core/Platform.h"

#if defined(LAMBDA_PLAT_WINDOWS)
    #include "Platform/Windows/WindowsPlatform.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/macOSPlatform.h"
#endif

namespace Lambda
{
    //--------
    //Platform
    //--------
    namespace Platform
	{
		void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            WindowsPlatform::MessageBox(pCaption, pText, type);
#elif defined(LAMBDA_PLAT_MACOS)
            macOSPlatform::MessageBox(pCaption, pText, type);
#endif
        }


		void SetCursorPosition(int32 x, int32 y)
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            WindowsPlatform::SetCursorPosition(x, y);
#elif defined(LAMBDA_PLAT_MACOS)
            macOSPlatform::SetCursorPosition(x, y);
#endif
        }


		void GetCursorPosition(int32& outX, int32& outY)
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            WindowsPlatform::GetCursorPosition(outX, outY);
#elif defined(LAMBDA_PLAT_MACOS)
            macOSPlatform::GetCursorPosition(outX, outY);
#endif
        }
	}
}