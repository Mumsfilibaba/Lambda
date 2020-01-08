#include "LambdaPch.h"

#include "Core/LambdaMain.h"

#include "Core/Engine/Console.h"

//----------------
//_CreateGameLayer
//----------------

LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)() = nullptr;

namespace Lambda
{
	//----------
	//LambdaMain
	//----------

	int32 LambdaMain(const SEngineParams& params)
	{
		if (!CEngine::Initialize(params))
		{
			return -1;
		}

		CEngine::Get().RunMainLoop();
		CEngine::Release();
		return 0;
	}
}