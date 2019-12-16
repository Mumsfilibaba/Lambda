#pragma once
#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
#include "Core/Input/IGamepadController.h"
#include "CXinputGamepadController.h"
#pragma comment(lib, "Xinput.lib")

#define XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE  7849
#define XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 8689
#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD    30

namespace Lambda
{
	//------------------------
	//CXInputGamepadController
	//------------------------

	CXInputGamepadController::CXInputGamepadController()
		: IGamepadController(),
		m_ControllerState(),
		m_GamepadStates(),
		m_PollingTimer(),
		m_CurrentPollRate(),
		m_PollRate(CTime::Seconds(1.0f/60.0f))
	{
	}


	bool CXInputGamepadController::IsConnected(EGamepad gamepadID) const
	{
		return m_GamepadStates[uint32(gamepadID)].IsConnected();
	}


	float CXInputGamepadController::GetTrigger(EGamepad gamepadID, EGamepadTrigger trigger) const
	{
		return m_GamepadStates[uint32(gamepadID)].GetTrigger(trigger);
	}


	const Vec2& CXInputGamepadController::GetThumbStick(EGamepad gamepadID, EGamepadStick stick) const
	{
		return m_GamepadStates[uint32(gamepadID)].GetThumbStick(stick);
	}


	CGamepadState CXInputGamepadController::GetGamepadState(EGamepad gamepadID) const
	{
		//Should we poll for input or return
		uint32 index = uint32(gamepadID);		
		m_PollingTimer.Tick();
		if (m_PollingTimer.GetTotalTime() >= m_CurrentPollRate)
		{
			m_PollingTimer.Reset();
		}
		else
		{
			return m_GamepadStates[index];
		}

		//Update
		bool hasGamepad = false;
		XINPUT_STATE state = {};
		for (uint32 i = 0; i < XUSER_MAX_COUNT; i++)
		{
			uint32 ID = GAMEPAD_1+i;
			memset(&state, 0, sizeof(state));

			DWORD result = XInputGetState(i, &state);
			if (result == ERROR_SUCCESS)
			{
				if (state.dwPacketNumber != m_ControllerState[i].dwPacketNumber)
				{
					//Set ID
					m_GamepadStates[ID].SetGamepadID(gamepadID);
					m_GamepadStates[ID].SetIsConnected(true);

					//triggers
					if (state.Gamepad.bLeftTrigger != m_ControllerState[i].Gamepad.bLeftTrigger)
						m_GamepadStates[ID].SetTrigger(GAMEPAD_TRIGGER_LEFT, state.Gamepad.bLeftTrigger);
					if (state.Gamepad.bRightTrigger != m_ControllerState[i].Gamepad.bRightTrigger)
						m_GamepadStates[ID].SetTrigger(GAMEPAD_TRIGGER_RIGHT, state.Gamepad.bLeftTrigger);

					//Check thumbs
					if (state.Gamepad.sThumbLX != m_ControllerState[i].Gamepad.sThumbLX || state.Gamepad.sThumbLY != m_ControllerState[i].Gamepad.sThumbLY)
						m_GamepadStates[ID].SetThumbStick(GAMEPAD_STICK_LEFT, Vec2(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY));
					if (state.Gamepad.sThumbRX != m_ControllerState[i].Gamepad.sThumbRX || state.Gamepad.sThumbRY != m_ControllerState[i].Gamepad.sThumbRY)
						m_GamepadStates[ID].SetThumbStick(GAMEPAD_STICK_RIGHT, Vec2(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY));

					//Buttons

					//TODO: Fix buttons

					//We found a controller
					hasGamepad = true;
					memcpy(&m_ControllerState[i], &state, sizeof(XINPUT_STATE));
				}
			}
			else if (result == ERROR_DEVICE_NOT_CONNECTED)
			{
				m_GamepadStates[ID].SetIsConnected(false);
			}
		}
		
		//Check again in two seconds if we does not have any gamepad connected
		m_CurrentPollRate = (hasGamepad) ? m_PollRate : CTime::Seconds(2.0f);
		return m_GamepadStates[index];
	}

}
#endif