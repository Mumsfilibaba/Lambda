#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //------------------
    //SWindowResizeEvent
    //------------------

    struct SWindowResizeEvent
    {
        uint32 Width;
        uint32 Height;
    };

    //----------------
    //SWindowMoveEvent
    //----------------

    struct SWindowMoveEvent
    {
        uint32 x;
        uint32 y;
    };

    //------------------------
    //SWindowFocusChangedEvent
    //------------------------

    struct SWindowFocusChangedEvent
    {
        bool bHasFocus = false;
    };
}
