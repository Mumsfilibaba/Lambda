#include "SandboxGame.h"

#include <Core/EntryPoint.h>

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