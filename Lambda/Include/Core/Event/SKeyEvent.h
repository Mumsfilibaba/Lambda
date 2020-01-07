#pragma once
#include "Core/Input/EKeyboard.h"

namespace Lambda
{
    //---------
    //SKeyEvent 
    //---------
    struct SKeyEvent
    {
        EKey Key;
        uint32 RepeatCount;
        uint32 Modifiers;
    };

    //-------------
    //SKeyTextEvent
    //-------------
    struct SKeyTextEvent 
    {
        uint32 Character;
    };
}
