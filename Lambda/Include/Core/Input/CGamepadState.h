#pragma once
#include "EGamepads.h"

namespace Lambda
{
    //-------------
    //CGamepadState
    //-------------

    class LAMBDA_API CGamepadState final
    {
    public:
        CGamepadState();
        CGamepadState(const CGamepadState& other);
        CGamepadState(CGamepadState&& other);
        ~CGamepadState() = default;
        
        CGamepadState& operator=(const CGamepadState& other);
        CGamepadState& operator=(CGamepadState&& other);
        
        _forceinline EGamepad GetGamepadID() const
        {
            return m_Gamepad;
        }
    private:
        EGamepad m_Gamepad;
    };
}
