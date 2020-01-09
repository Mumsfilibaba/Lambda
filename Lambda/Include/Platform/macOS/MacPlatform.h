#pragma once
#include "LambdaCore.h"

#include "Core/Platform.h"
#include "Core/Input/EMouse.h"
#include "Core/Input/EKeyboard.h"

namespace Lambda
{
    //-----------
    //MacPlatform
    //-----------

    namespace MacPlatform
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
    
        EMouseButton ConvertMouseButton(uint32 button);
    }
}
