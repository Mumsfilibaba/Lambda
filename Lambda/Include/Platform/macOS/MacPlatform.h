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
        void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);
    
        uint64 QueryPerformanceFrequency();
        uint64 QueryPerformanceCounter();
    
        void CreateKeyLookupTable();
        EKey ConvertFromVirtualKey(uint32 keycode);
        uint32 ConvertToVirtualKey(EKey keycode);
        uint32 ConvertModifierKeyFlags(uint32 flags);
        EMouseButton ConvertMouseButton(uint32 button);
    }
}
