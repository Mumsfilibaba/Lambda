#pragma once
#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/CEngine.h"

extern LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)(void);

int main(int argc, const char* argv[])
{
    using namespace Lambda;
    
    //Initialize CreateGameLayerFunc
    _CreateGameLayer = Lambda::CreateGameLayer;
    
    //Start and initialize engine
	Lambda::SEngineParams params = {};
	params.ppCmdArgs		= nullptr;
	params.CmdArgsCount		= 0;
	return Lambda::LambdaMain(params);
}
#endif
