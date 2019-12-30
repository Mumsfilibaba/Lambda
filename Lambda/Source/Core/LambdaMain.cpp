#include "LambdaPch.h"

#include "Core/LambdaMain.h"

//----------------
//_CreateGameLayer
//----------------

LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)(Lambda::IEngine*) = nullptr;

namespace Lambda
{
	//----------
	//LambdaMain
	//----------

	int32 LambdaMain(const SEngineParams& params)
	{
		if (IEngine::Initialize(params))
		{
			IEngine* pEngine = IEngine::Get();
			if (pEngine)
			{
				pEngine->RunMainLoop();
				pEngine->Release();

				return 0;
			}

			return -1;
		}

		return -1;
	}
}