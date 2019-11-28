#pragma once
#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/LEngine.h"

extern LAMBDA_API Lambda::Layer* (*_CreateGameLayer)(void);

int main(int argc, const char* argv[])
{
    using namespace Lambda;
    
    //Initialize CreateGameLayerFunc
    _CreateGameLayer = Lambda::CreateGameLayer;
    
    //Start and initialize engine
	Lambda::LEngineParams params = {};
	params.ppCmdArgs		= nullptr;
	params.CmdArgsCount		= 0;
	return Lambda::LambdaMain(params);
}
#endif
