#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Events/EventDispatcher.h"
	#include "WindowsJoystickManager.h"
	
	#pragma comment(lib, "Xinput.lib")

namespace Lambda
{
	//---------------
	//JoystickManager
	//---------------

	JoystickManager* JoystickManager::Create()
	{
		return DBG_NEW WindowsJoystickManager();
	}
	
	//----------------------
	//WindowsJoystickManager
	//----------------------

	WindowsJoystickManager::WindowsJoystickManager()
	{
		memset(&m_ControllerState, 0, sizeof(XINPUT_STATE) * XUSER_MAX_COUNT);
		m_PollRate = Timestep::Seconds(1.0f / 60.0f);
		m_CurrentPollRate = m_PollRate;
	}


	void WindowsJoystickManager::InternalOnUpdate()
	{
		//Should we poll for input or return
		m_PollingTimer.Tick();
		if (m_PollingTimer.GetTotalTime() >= m_CurrentPollRate)
		{
			m_PollingTimer.Reset();
		}
		else
		{
			return;
		}

		//Update
		bool isConnected = false;
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
					event.JoystickChanged.ControllerID = uint8(i);

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
					//TODO: Fix buttons

					EventDispatcher::DispatchEvent(event);
					memcpy(&(m_ControllerState[i]), &state, sizeof(XINPUT_STATE));

					//We found a controller
					isConnected = true;
				}
			}
		}

		//Check again in two seconds if we have connnected
		if (!isConnected)
		{
			m_CurrentPollRate = Timestep::Seconds(2.0f);
		}
		else
		{
			m_CurrentPollRate = m_PollRate;
		}
	}


	void WindowsJoystickManager::InternalSetPollrate(const Timestep& time)
	{
		m_PollRate = time;
	}


	Timestep WindowsJoystickManager::InternalGetPollrate() const
	{
		return m_PollRate;
	}
}
#endif
