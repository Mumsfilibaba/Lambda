#pragma once
#include "LambdaCore.h"

//Undefine the Win32 MessageBox macro
#ifdef MessageBox
    #undef MessageBox
#endif

//Define build string
#if defined(LAMBDA_DEBUG)
    #define LAMBDA_BUILD "Debug"
#elif defined(LAMBDA_RELEASE)
    #define LAMBDA_BUILD "Release"
#elif defined(LAMBDA_PRODUCTION)
    #define LAMBDA_BUILD "Production"
#endif

//Define architecture string
#if defined(LAMBDA_X64)
    #define LAMBDA_ARCHITECTURE "x64"
#endif

//Define platform string
#if defined(LAMBDA_PLAT_WINDOWS)
    #define LAMBDA_PLATFORM "Windows"
#elif defined(LAMBDA_PLAT_MACOS)
    #define LAMBDA_PLATFORM "macOS"
#endif

namespace Lambda
{
    //---------------
    //EMessageBoxType
    //---------------
    enum class EMessageBoxType : uint32
    {   
        MESSAGE_BOX_TYPE_UNKNOWN    = 0,
        MESSAGE_BOX_TYPE_ERROR      = 1,
    };

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
            void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);
        }
        
        //----
        //Time
        //----
        namespace Time
        {
            uint64 QueryPerformanceFrequency();
            uint64 QueryPerformanceCounter();
        }
    }
}
