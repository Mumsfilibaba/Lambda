#include "LambdaPch.h"
#include "Core/Input/CGamepadState.h"

namespace Lambda
{
	//-------------
	//CGamepadState
	//-------------

	CGamepadState::CGamepadState()
		: m_Gamepad(GAMEPAD_UNKNOWN),
		m_IsConnected(false),
		m_Triggers(),
		m_Sticks()
	{
	}


	CGamepadState::CGamepadState(const CGamepadState& other)
		: m_Gamepad(GAMEPAD_UNKNOWN),
		m_IsConnected(false),
		m_Triggers(),
		m_Sticks()
	{
		memcpy(this, &other, sizeof(CGamepadState));
	}


	CGamepadState::CGamepadState(CGamepadState&& other)
		: m_Gamepad(GAMEPAD_UNKNOWN),
		m_IsConnected(false),
		m_Triggers(),
		m_Sticks()
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
		LAMBDA_ASSERT(stick <= GAMEPAD_STICK_LAST);
		m_Sticks[stick] = value;
	}


	void CGamepadState::SetGamepadID(EGamepad gamepadID)
	{
		m_Gamepad = gamepadID;
	}


	void CGamepadState::SetIsConnected(bool isConnected)
	{
		m_IsConnected = isConnected;
	}
}
