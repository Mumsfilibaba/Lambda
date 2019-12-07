#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class CGamepadState final
    {
    public:
        CGamepadState();
        CGamepadState(const CGamepadState& other);
        CGamepadState(CGamepadState&& other);
        ~CGamepadState() = default;
    };
}
