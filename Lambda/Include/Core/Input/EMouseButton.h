#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //------------
    //EMouseButton
    //------------

    enum EMouseButton : uint8
    {
        MOUSEBUTTON_UNKNOWN     = 0,
        MOUSEBUTTON_LEFT        = 1,
        MOUSEBUTTON_MIDDLE      = 2,
        MOUSEBUTTON_RIGHT       = 3,
        MOUSEBUTTON_FORWARD     = 4,
        MOUSEBUTTON_BACKWARD    = 5,
		MOUSEBUTTON_LAST		= MOUSEBUTTON_BACKWARD,
    };
}
