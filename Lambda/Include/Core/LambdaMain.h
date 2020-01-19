#pragma once
#include "Engine/Engine.h"

namespace Lambda
{
	class CLayer;
	struct SEngineParams;

	//---------------
	//CreateGameLayer
	//---------------
	CLayer* CreateGameLayer();

	//----------
	//LambdaMain
	//----------
	int32 LAMBDA_API LambdaMain(const SEngineParams& engineParams);
}