#pragma once
#include "Core/Input/EMouse.h"

namespace Lambda
{
    //----------------
    //MouseButtonEvent
    //----------------
    
    struct MouseButtonEvent
    {
        EMouseButton Button;
        uint32 Modifiers;
    };

    //---------------
    //MouseMovedEvent
    //---------------
    
    struct MouseMovedEvent
    {
        int32 x;
        int32 y;
    };

    //------------------
    //MouseScrolledEvent
    //------------------
    
    struct MouseScrolledEvent 
    {
        float Horizontal;
        float Vertical;
    };
}
