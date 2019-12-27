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

		virtual bool IsConnected(EGamepad gamepadID) const = 0;
		virtual float GetTrigger(EGamepad gamepadID, EGamepadTrigger trigger) const = 0;
		virtual const Vec2& GetThumbStick(EGamepad gamepadID, EGamepadStick stick) const = 0;
        virtual CGamepadState GetGamepadState(EGamepad gamepadID) const = 0;
    };

	//-------------------------------------------------------------------
	//CDummyGamepadController - Used when no real controller is available
	//-------------------------------------------------------------------
	class CDummyGamepadController final : public IGamepadController
	{
	public:
		CDummyGamepadController() = default;
		~CDummyGamepadController() = default;

		virtual bool IsConnected(EGamepad) const
		{
			return false;
		}

		virtual float GetTrigger(EGamepad, EGamepadTrigger) const 
		{
			return 0.0f;
		}
		
		virtual const Vec2& GetThumbStick(EGamepad, EGamepadStick) const
		{
			return m_ThumbStick;
		}

		virtual CGamepadState GetGamepadState(EGamepad) const override final
		{
			return CGamepadState();
		}
	private:
		Vec2 m_ThumbStick;
	};
}
