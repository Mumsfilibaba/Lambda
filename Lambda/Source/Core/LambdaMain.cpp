#include "LambdaPch.h"

#include "Core/LambdaMain.h"

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
		CEngine* pEngine = CEngine::Create();
		if (!pEngine->Init(params))
		{
			return -1;
		}

		pEngine->Startup();
        while (pEngine->IsRunning())
			pEngine->Tick();
        
		pEngine->Release();

		int32 result = pEngine->GetExitCode();
		SafeDelete(pEngine);
		return result;
	}
}
