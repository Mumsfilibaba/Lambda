#pragma once
#include "LambdaCore.h"

#include "Core/Platform.h"

namespace Lambda
{
    //-----------
    //MacPlatform
    //-----------

    namespace MacPlatform
    {
        void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);
    
        uint64 QueryPerformanceFrequency();
        uint64 QueryPerformanceCounter();
    }
}
