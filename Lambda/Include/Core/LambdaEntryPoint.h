#pragma once
#include "LambdaCore.h"
#include "LambdaMain.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsEntrypoint.inl"
#else
	extern Lambda::CLayer* (*_CreateGameLayer)(Lambda::IEngine*);

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
