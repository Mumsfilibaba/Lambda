#pragma once
#include "Engine/Engine.h"

namespace Lambda
{
	class CLayer;
	class IEngine;
	struct SEngineParams;

	//----------
	//LambdaMain
	//----------
	int32 LAMBDA_API LambdaMain(const SEngineParams& engineParams);

	//---------------
	//CreateGameLayer
	//---------------
	CLayer* CreateGameLayer(IEngine* pEngine);
}