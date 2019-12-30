#pragma once
#include "LambdaCore.h"

//Undefine the Win32 MessageBox macro
#ifdef MessageBox
    #undef MessageBox
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

    //---------
    //CPlatform
    //---------
    class CPlatform
    {
    public:
        LAMBDA_STATIC_CLASS(CPlatform);

        static void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);
        
        static uint64 QueryPerformanceFrequency();
        static uint64 QueryPerformanceCounter();
    };
}