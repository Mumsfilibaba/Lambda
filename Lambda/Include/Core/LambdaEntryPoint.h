#pragma once
#include "LambdaMain.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsEntrypoint.inl"
#else
	int main(int, const char*[])
	{
        using namespace Lambda;
        
		_CreateGameLayer = CreateGameLayer;

		SEngineParams params = {};
		params.ppCmdLineArgs 	 = nullptr;
		params.nCmdLineArgsCount = 0;
		LambdaMain(params);
		return 0;
	}
#endif
