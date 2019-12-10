#pragma once
#include "EGamepad.h"

namespace Lambda
{
    //-------------
    //CGamepadState
    //-------------

    class LAMBDA_API CGamepadState final
    {
    public:
        CGamepadState() = default;
        CGamepadState(const CGamepadState& other) = default;
        CGamepadState(CGamepadState&& other) = default;
        ~CGamepadState() = default;
        
        CGamepadState& operator=(const CGamepadState& other) = default;
        CGamepadState& operator=(CGamepadState&& other) = default;
        
        _forceinline EGamepad GetGamepadID() const
        {
            return m_Gamepad;
        }
    private:
        EGamepad m_Gamepad;
    };
}
