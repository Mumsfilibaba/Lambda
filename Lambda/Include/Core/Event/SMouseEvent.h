#pragma once
#include "../Input/EKey.h"
#include "../Input/EMouseButton.h"

namespace Lambda
{
    //-----------------
    //SMouseButtonEvent
    //-----------------

    struct SMouseButtonEvent
    {
        EMouseButton Button;
        uint32 Modifiers;
    };

    //----------------
    //SMouseMovedEvent
    //----------------

    struct SMouseMovedEvent
    {
        int32 x;
        int32 y;
    };

    //-------------------
    //SMouseScrolledEvent
    //-------------------

    struct SMouseScrolledEvent 
    {
        float Horizontal;
        float Vertical;
    };
}
