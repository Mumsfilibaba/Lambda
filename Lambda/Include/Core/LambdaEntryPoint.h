#pragma once
#include "LambdaMain.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsEntrypoint.inl"
#else
    extern Lambda::Layer* (*_CreateGameLayer)();

	int main(int, const char*[])
	{
        using namespace Lambda;
        
		_CreateGameLayer = CreateGameLayer;

		EngineParams params = {};
		params.ppCmdLineArgs 	= nullptr;
		params.CmdLineArgsCount = 0;
		LambdaMain(params);
		return 0;
	}
#endif
