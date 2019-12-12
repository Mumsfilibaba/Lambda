#pragma once
#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
#include "Core/Input/IGamepadController.h"
#include "CXinputGamepadController.h"
#pragma comment(lib, "Xinput.lib")

namespace Lambda
{
	//------------------------
	//CXInputGamepadController
	//------------------------

	CXInputGamepadController::CXInputGamepadController()
		: IGamepadController(),
		m_ControllerState(),
		m_GamepadStates()
	{
	}


	bool CXInputGamepadController::IsConnected(EGamepad gamepadID) const
	{
		return false;
	}


	CGamepadState CXInputGamepadController::GetGamepadState(EGamepad gamepadID) const
	{
		//Update
		bool isConnected = false;

		XINPUT_STATE state = {};
		memset(&state, 0, sizeof(state));

		uint32 index = uint32(gamepadID);
		if (XInputGetState(index, &state) == ERROR_SUCCESS)
		{
			if (state.dwPacketNumber != m_ControllerState[index].dwPacketNumber)
			{
				//Set ID
				m_GamepadStates[index].SetGamepadID(gamepadID);

				//triggers
				if (state.Gamepad.bLeftTrigger != m_ControllerState[index].Gamepad.bLeftTrigger)
					m_GamepadStates[index].SetTrigger(GAMEPAD_TRIGGER_LEFT, state.Gamepad.bLeftTrigger);
				if (state.Gamepad.bRightTrigger != m_ControllerState[index].Gamepad.bRightTrigger)
					m_GamepadStates[index].SetTrigger(GAMEPAD_TRIGGER_RIGHT, state.Gamepad.bLeftTrigger);

				//Check thumbs
				if (state.Gamepad.sThumbLX != m_ControllerState[index].Gamepad.sThumbLX || state.Gamepad.sThumbLY != m_ControllerState[index].Gamepad.sThumbLY)
					m_GamepadStates[index].SetThumbStick(GAMEPAD_STICK_LEFT, Vec2(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY));
				if (state.Gamepad.sThumbRX != m_ControllerState[index].Gamepad.sThumbRX || state.Gamepad.sThumbRY != m_ControllerState[index].Gamepad.sThumbRY)
					m_GamepadStates[index].SetThumbStick(GAMEPAD_STICK_RIGHT, Vec2(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY));
				
				//Buttons

				//TODO: Fix buttons

				//We found a controller
				isConnected = true;
			}
		}
		
		return m_GamepadStates[index];
	}

}
#endif