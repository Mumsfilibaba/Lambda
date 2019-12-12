#pragma once
#include "Core/Input/IGamepadController.h"
#if defined(LAMBDA_PLAT_WINDOWS)
#include "Time/CClock.h"
#include <Xinput.h>

namespace Lambda
{
	//------------------------
	//CXInputGamepadController
	//------------------------

	class CXInputGamepadController final : public IGamepadController
	{
	public:
		CXInputGamepadController();
		~CXInputGamepadController() = default;

		virtual bool IsConnected(EGamepad gamepadID) const override final;
		virtual CGamepadState GetGamepadState(EGamepad gamepadID) const override final;
	private:
		XINPUT_STATE m_ControllerState[XUSER_MAX_COUNT];
		mutable CGamepadState m_GamepadStates[GAMEPAD_LAST+1];
	};
}
#endif