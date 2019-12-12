#include "LambdaPch.h"
#include "Core/Input/CGamepadState.h"

namespace Lambda
{
	//-------------
	//CGamepadState
	//-------------

	CGamepadState::CGamepadState()
	{
	}


	CGamepadState::CGamepadState(const CGamepadState& other)
	{
		memcpy(this, &other, sizeof(CGamepadState));
	}


	CGamepadState::CGamepadState(CGamepadState&& other)
	{
		memcpy(this, &other, sizeof(CGamepadState));
	}


	CGamepadState& CGamepadState::operator=(const CGamepadState& other)
	{
		if (this != &other)
			memcpy(this, &other, sizeof(CGamepadState));

		return *this;
	}


	CGamepadState& CGamepadState::operator=(CGamepadState&& other)
	{
		if (this != &other)
			memcpy(this, &other, sizeof(CGamepadState));

		return *this;
	}


	void CGamepadState::SetTrigger(EGamepadTrigger trigger, float value)
	{
		LAMBDA_ASSERT(trigger <= GAMEPAD_TRIGGER_LAST);
		m_Triggers[trigger] = value;
	}


	void CGamepadState::SetThumbStick(EGamepadStick stick, Vec2 value)
	{
		LAMBDA_ASSERT(stick <= GAMEPAD_LAST);
		m_Sticks[stick] = value;
	}


	void CGamepadState::SetGamepadID(EGamepad gamepadID)
	{
		m_Gamepad = gamepadID;
	}
}