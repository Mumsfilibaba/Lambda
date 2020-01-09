#pragma once
#include "Core/Input/EKeyboard.h"

namespace Lambda
{
    //--------
    //KeyEvent 
    //--------
    
    struct KeyEvent
    {
        EKey Key;
        uint32 RepeatCount;
        uint32 Modifiers;
    };

    //------------
    //KeyTextEvent
    //------------
    
    struct KeyTextEvent 
    {
        uint32 Character;
    };
}
