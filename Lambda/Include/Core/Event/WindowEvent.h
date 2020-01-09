#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //-----------
    //EResizeType
    //-----------

    enum class EResizeType : uint32
    {
        RESIZE_TYPE_UNKNOWN     = 0,
        RESIZE_TYPE_MAXIMIZED   = 1,
        RESIZE_TYPE_MINIMIZED   = 2,
        RESIZE_TYPE_STANDARD    = 3
    };

    //-----------------
    //WindowResizeEvent
    //-----------------
    
    struct WindowResizeEvent
    {
        EResizeType ResizeType;
        uint32 Width;
        uint32 Height;
    };

    //---------------
    //WindowMoveEvent
    //---------------
    
    struct WindowMoveEvent
    {
        uint32 x;
        uint32 y;
    };

    //-----------------------
    //WindowFocusChangedEvent
    //-----------------------

    struct WindowFocusChangedEvent
    {
        bool bHasFocus = false;
    };
}
