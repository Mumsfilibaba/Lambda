#pragma once
#include "Core/Event/EKey.h"
#include "Core/Event/EMouseButton.h"

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
