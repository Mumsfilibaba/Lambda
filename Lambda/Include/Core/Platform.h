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
