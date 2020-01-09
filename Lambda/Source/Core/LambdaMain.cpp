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

	extern Engine g_Engine;

	int32 LambdaMain(const EngineParams& params)
	{
		if (!g_Engine.Init(params))
		{
			return -1;
		}

		g_Engine.RunMainLoop();
		g_Engine.Release();
		return g_Engine.GetExitCode();
	}
}