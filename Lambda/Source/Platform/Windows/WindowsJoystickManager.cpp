#include <LambdaPch.h>

#if defined(LAMBDA_PLAT_WINDOWS)
	#include <System/EventDispatcher.h>
	#include "WindowsJoystickManager.h"

	#pragma comment(lib, "Xinput.lib")

namespace Lambda
{
	JoystickManager* JoystickManager::Create()
	{
		return new WindowsJoystickManager();
	}
	
	WindowsJoystickManager::WindowsJoystickManager()
	{
		memset(&m_ControllerState, 0, sizeof(XINPUT_STATE) * XUSER_MAX_COUNT);
	}
	
	WindowsJoystickManager::~WindowsJoystickManager()
	{
	}

	void WindowsJoystickManager::UpdateJoystickState()
	{
		for (uint32 i = 0; i < XUSER_MAX_COUNT; i++)
		{
			XINPUT_STATE state = {};
			memset(&state, 0, sizeof(state));

			if (XInputGetState(i, &state) == ERROR_SUCCESS)
			{
				if (state.dwPacketNumber != m_ControllerState[i].dwPacketNumber)
				{
					Event event = {};
					memset(&event, 0, sizeof(Event));

					event.Type = EVENT_TYPE_JOYSTICK_CHANGED;
					event.JoystickChanged.ControllerID = i;

					//Check triggers
					if (state.Gamepad.bLeftTrigger != m_ControllerState[i].Gamepad.bLeftTrigger)
						event.JoystickChanged.LeftTrigger = state.Gamepad.bLeftTrigger;
					if (state.Gamepad.bRightTrigger != m_ControllerState[i].Gamepad.bRightTrigger)
						event.JoystickChanged.RightTrigger = state.Gamepad.bRightTrigger;
					//Check thumbs
					if (state.Gamepad.sThumbLX != m_ControllerState[i].Gamepad.sThumbLX)
						event.JoystickChanged.LeftX = state.Gamepad.sThumbLX;
					if (state.Gamepad.sThumbLY != m_ControllerState[i].Gamepad.sThumbLY)
						event.JoystickChanged.LeftY = state.Gamepad.sThumbLY;
					if (state.Gamepad.sThumbRX != m_ControllerState[i].Gamepad.sThumbRX)
						event.JoystickChanged.RightX = state.Gamepad.sThumbRX;
					if (state.Gamepad.sThumbRY != m_ControllerState[i].Gamepad.sThumbRY)
						event.JoystickChanged.RightY = state.Gamepad.sThumbRY;
					//Buttons


					EventDispatcher::SendEvent(event);

					memcpy(&(m_ControllerState[i]), &state, sizeof(XINPUT_STATE));
				}
			}
		}
	}
}
#endif