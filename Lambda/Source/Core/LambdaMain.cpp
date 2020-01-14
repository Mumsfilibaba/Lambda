#include "LambdaPch.h"

#include "Core/LambdaMain.h"

//----------------
//_CreateGameLayer
//----------------

LAMBDA_API Lambda::Layer* (*_CreateGameLayer)() = nullptr;

namespace Lambda
{
	//----------
	//LambdaMain
	//----------

	extern CEngine g_Engine;

	int32 LambdaMain(const SEngineParams& params)
	{
		if (!g_Engine.Init(params))
		{
			return -1;
		}

        while (g_Engine.IsRunning())
            g_Engine.Tick();
        
		g_Engine.Release();
		return g_Engine.GetExitCode();
	}
}
