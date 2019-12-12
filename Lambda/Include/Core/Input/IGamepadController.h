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

		virtual CGamepadState GetGamepadState(EGamepad) const override final
		{
			return CGamepadState();
		}
	};
}
