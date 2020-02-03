#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsInput.h"
	#include "Platform/Windows/WindowsWindow.h"

#include "Core/Log/EngineLog.h"

	#include <Xinput.h>

	#define STICK_MAX_VALUE			32767
	#define LEFT_STICK_DEADZONE		7849
	#define RIGHT_STICK_DEADZONE	8269
	#define TRIGGER_DEADZONE		30
	#define TRIGGER_MAX_VALUE		255

namespace Lambda
{
	SMouseState CWindowsInput::s_MouseState;
	SKeyboardState CWindowsInput::s_KeyboardState;
	SGamepadState CWindowsInput::s_GamepadStates[EGamepad::GAMEPAD_COUNT];
	DWORD CWindowsInput::s_GamepadPacketIDs[EGamepad::GAMEPAD_COUNT];

	void CWindowsInput::Init()
	{
		//Init keytable
		CWindowsKeyboard::Init();

		//Init gamepadstates
		for (uint32 i = 0; i < EGamepad::GAMEPAD_COUNT; i++)
			s_GamepadStates[i].Player = EGamepad(i + 1);
	}
	
	void CWindowsInput::Update()
	{
		//This compile-time count takes the smaller count
		constexpr uint32 gamepadCount = std::min<uint32>(XUSER_MAX_COUNT, EGamepad::GAMEPAD_COUNT);

		//Get all gamepads state
		XINPUT_STATE state = {};
		for (uint32 i = 0; i < gamepadCount; i++)
		{
			SGamepadState& gamepadState = s_GamepadStates[i];
			memset(&state, 0, sizeof(XINPUT_STATE));
			
			if (XInputGetState(i, &state) == ERROR_SUCCESS)
			{
				//Check the packet number of this gamepad, if it is the same there is not state to update
				if (s_GamepadPacketIDs[i] != state.dwPacketNumber)
				{
					s_GamepadPacketIDs[i] = state.dwPacketNumber;
					gamepadState.bIsConnected = true;

					//Get stick info
					XINPUT_GAMEPAD& xiGamepad = state.Gamepad;

					float magnitude = float(xiGamepad.sThumbLX * xiGamepad.sThumbLX) + float(xiGamepad.sThumbLY * xiGamepad.sThumbLY);
					magnitude = sqrtf(magnitude);
					if (magnitude > LEFT_STICK_DEADZONE)
					{
						magnitude = std::min<float>(magnitude, float(STICK_MAX_VALUE));

						gamepadState.LeftStick.x = xiGamepad.sThumbLX / magnitude;
						gamepadState.LeftStick.y = xiGamepad.sThumbLY / magnitude;
					}
					else
					{
						gamepadState.LeftStick.x = 0.0f;
						gamepadState.LeftStick.y = 0.0f;
					}

					magnitude = float(xiGamepad.sThumbRX * xiGamepad.sThumbRX) + float(xiGamepad.sThumbRY * xiGamepad.sThumbRY);
					magnitude = sqrtf(magnitude);
					if (magnitude > RIGHT_STICK_DEADZONE)
					{
						magnitude = std::min<float>(magnitude, float(STICK_MAX_VALUE));
						
						gamepadState.RightStick.x = xiGamepad.sThumbRX / magnitude;
						gamepadState.RightStick.y = xiGamepad.sThumbRY / magnitude;
					}
					else
					{
						gamepadState.RightStick.x = 0.0f;
						gamepadState.RightStick.y = 0.0f;
					}

					//Get button info
					memset(gamepadState.bButtonStates, 0, sizeof(gamepadState.bButtonStates));
					if (xiGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_PAD_UP] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_PAD_DOWN] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_PAD_RIGHT] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_PAD_LEFT] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_Y)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_FACE_UP] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_A)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_FACE_DOWN] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_B)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_FACE_RIGHT] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_X)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_FACE_LEFT] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_START)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_START] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_BACK)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_SELECT] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_LEFT_TRIGGER] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_RIGHT_TRIGGER] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_LEFT_STICK] = true;

					if (xiGamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
						gamepadState.bButtonStates[GAMEPAD_BUTTON_RIGHT_STICK] = true;

					//Get trigger info
					constexpr float triggerDenom = (TRIGGER_MAX_VALUE - TRIGGER_DEADZONE);
					if (xiGamepad.bLeftTrigger > TRIGGER_DEADZONE)
						gamepadState.LeftTrigger = (float)(xiGamepad.bLeftTrigger - TRIGGER_DEADZONE) / triggerDenom;
					else
						gamepadState.LeftTrigger = 0;

					if (xiGamepad.bRightTrigger > TRIGGER_DEADZONE)
						gamepadState.RightTrigger = (float)(xiGamepad.bRightTrigger - TRIGGER_DEADZONE) / triggerDenom;
					else
						gamepadState.RightTrigger = 0;
				}
			}
			else
			{
				if (gamepadState.bIsConnected)
				{
					memset(&gamepadState, 0, sizeof(SGamepadState));

					gamepadState.Player = EGamepad(i + 1);
					gamepadState.bIsConnected = false;
				}
			}
		}
	}

	void CWindowsInput::SetMousePosition(IWindow* pRelativeWindow, int32 x, int32 y)
	{
		LAMBDA_ASSERT(pRelativeWindow != nullptr);

		HWND hWindow = reinterpret_cast<CWindowsWindow*>(pRelativeWindow)->GetHandle();
		
		POINT point = {};
		point.x = x;
		point.y = y;

		::ScreenToClient(hWindow, &point);
		::SetCursorPos(point.x, point.y);
	}

	void CWindowsInput::GetMousePosition(IWindow* pRelativeWindow, int32& x, int32& y)
	{
		LAMBDA_ASSERT(pRelativeWindow != nullptr);

		HWND hWindow = reinterpret_cast<CWindowsWindow*>(pRelativeWindow)->GetHandle();

		POINT point = {};
		::GetCursorPos(&point);
		::ClientToScreen(hWindow, &point);

		x = point.x;
		y = point.y;
	}
	
	void CWindowsInput::OnKeyboardMessage(uint32 virtualKey, bool bKeyDown)
	{
		EKey key = CWindowsKeyboard::ConvertVirtualKey(virtualKey);
		s_KeyboardState.bKeyStates[key] = bKeyDown;
	}
	
	void CWindowsInput::OnMouseButtonMessage(uint32 message, WPARAM wParam)
	{
		//Get button
		EMouseButton button = EMouseButton::MOUSE_BUTTON_UNKNOWN;
		if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
			button = EMouseButton::MOUSE_BUTTON_LEFT;
		else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
			button = EMouseButton::MOUSE_BUTTON_MIDDLE;
		else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
			button = EMouseButton::MOUSE_BUTTON_RIGHT;
		else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			button = EMouseButton::MOUSE_BUTTON_BACKWARD;
		else
			button = EMouseButton::MOUSE_BUTTON_FORWARD;

		//Set state
		if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP || message == WM_XBUTTONUP)
			s_MouseState.bButtonStates[button] = false;
		else
			s_MouseState.bButtonStates[button] = true;
	}
}
#endif