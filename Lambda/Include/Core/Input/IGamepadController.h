#pragma once
#include "CGamepadState.h"

namespace Lambda
{
    //------------------
    //IGamepadController
    //------------------

    class IGamepadController
    {
    public:
        LAMBDA_INTERFACE(IGamepadController);
        virtual CGamepadState GetGamepadState(EGamepad gamepadID) const = 0;
    };
}
