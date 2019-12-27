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
		virtual float GetTrigger(EGamepad gamepadID, EGamepadTrigger trigger) const override final;
		virtual const Vec2& GetThumbStick(EGamepad gamepadID, EGamepadStick stick) const override final;
		virtual CGamepadState GetGamepadState(EGamepad gamepadID) const override final;
	private:
		mutable XINPUT_STATE m_ControllerState[XUSER_MAX_COUNT];
		mutable CGamepadState m_GamepadStates[GAMEPAD_LAST+1];
		mutable CClock m_PollingTimer;
		mutable CTime m_CurrentPollRate;
		CTime m_PollRate;
	};
}
#endif