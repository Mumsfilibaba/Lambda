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
	CGame* CreateGameInstance()
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