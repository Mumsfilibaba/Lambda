#pragma once
#include "CGamepadState.h"

namespace Lambda
{
    //--------
    //CGamePad
    //--------
    class LAMBDA_API CGamepad final
    {
        friend class CEngine;
    public:
        LAMBDA_STATIC_CLASS(CGamepad);

		static float GetTrigger(EGamepad gamepadID, EGamepadTrigger trigger);
		static const Vec2& GetThumbStick(EGamepad gamepadID, EGamepadStick stick);
        static const CGamepadState& GetState(EGamepad gamepadID);
    private:
        static void UpdateState();
    private:
        static CGamepadState s_CurrentState[GAMEPAD_LAST];
        static CGamepadState s_LastState[GAMEPAD_LAST];
    };
}
