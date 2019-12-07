#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //--------
    //CGamePad
    //--------

    class LAMBDA_API CGamepadState final
    {
    public:
        CGamepadState();
        CGamepadState(const CGamepadState& other);
        CGamepadState(CGamepadState&& other);
        ~CGamepadState() = default;
    };
}
