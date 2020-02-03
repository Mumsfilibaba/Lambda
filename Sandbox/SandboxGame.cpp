#include "SandboxGame.h"

#include <Core/IWindow.h>
#include <Core/EntryPoint.h>
#include <Core/Log/EngineLog.h>

#include <Core/Input/Input.h>
#include <Core/Input/MouseState.h>
#include <Core/Input/GamepadState.h>
#include <Core/Input/KeyboardState.h>

namespace Lambda
{
	CGame* CreateGame()
	{
		return DBG_NEW CSandBoxGame();
	}

	CSandBoxGame::CSandBoxGame()
	{
	}

	void CSandBoxGame::Init()
	{
	}

	void CSandBoxGame::Update(const CTimestep&)
	{
		SGamepadState gamepadState = {};
		Input::GetGamepadState(gamepadState, EGamepad::GAMEPAD_PLAYER_1);

		if (gamepadState.bIsConnected)
		{
			//D_LOG_INFO("Left Stick x=%.6f, y=%.6f", gamepadState.LeftStick.x, gamepadState.LeftStick.y);
			D_LOG_INFO("Right Stick x=%.6f, y=%.6f", gamepadState.RightStick.x, gamepadState.RightStick.y);
			//D_LOG_INFO("Left Trigger=%.6f", gamepadState.LeftTrigger);
			//D_LOG_INFO("Right Trigger=%.6f", gamepadState.RightTrigger);

			if (gamepadState.IsButtonPressed(EGamepadButton::GAMEPAD_BUTTON_FACE_RIGHT))
				CEngine::Get().RequestExit();
		}
	}

	void CSandBoxGame::FixedUpdate(const CTimestep&)
	{
	}

	void CSandBoxGame::Render(const CTimestep&)
	{
	}

	void CSandBoxGame::Release()
	{
	}
}