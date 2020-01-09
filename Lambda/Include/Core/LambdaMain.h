#pragma once
#include "Engine/Engine.h"

namespace Lambda
{
	class Layer;
	class IEngine;
	struct EngineParams;

	//---------------
	//CreateGameLayer
	//---------------
	Layer* CreateGameLayer();

	//----------
	//LambdaMain
	//----------
	int32 LAMBDA_API LambdaMain(const EngineParams& engineParams);
}