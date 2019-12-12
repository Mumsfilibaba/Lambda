#pragma once
#include "EGamepad.h"
#include "Core/CLogManager.h"
#include <glm/glm.hpp>

namespace Lambda
{
	using Vec2 = glm::vec2;

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
        
		void SetTrigger(EGamepadTrigger trigger, float value);
		void SetThumbStick(EGamepadStick stick, Vec2 value);
		void SetGamepadID(EGamepad gamepadID);

		_forceinline float GetTrigger(EGamepadTrigger trigger) const
		{
			LAMBDA_ASSERT(trigger <= GAMEPAD_TRIGGER_LAST);
			return m_Triggers[trigger];
		}

		_forceinline const Vec2& GetThumbStick(EGamepadStick stick) const
		{
			LAMBDA_ASSERT(stick <= GAMEPAD_LAST);
			return m_Sticks[stick];
		}

        _forceinline EGamepad GetGamepadID() const
        {
            return m_Gamepad;
        }
    private:
        EGamepad m_Gamepad;
		float m_Triggers[GAMEPAD_TRIGGER_LAST+1];
		Vec2 m_Sticks[GAMEPAD_STICK_LAST+1];
    };
}
