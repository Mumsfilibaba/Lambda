#include "LambdaPch.h"

#include "Core\Game.h"

namespace Lambda
{
	CGame* CGame::s_pInstance = nullptr;
	LAMBDA_API CreateGameInstanceFunc _CreateGameInstance = nullptr;

	CGame::CGame()
	{
		s_pInstance = this;
	}
	
	CGame::~CGame()
	{
		s_pInstance = nullptr;
	}
}