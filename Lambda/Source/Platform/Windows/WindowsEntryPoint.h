#pragma once
#if defined(LAMBDA_PLAT_WINDOWS)
#include "Core/CEngine.h"

#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

extern LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)(void);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	using namespace Lambda;

	//Set function for createing gamelayer - Needed for DLL- and Static-Lib compilation
	_CreateGameLayer = CreateGameLayer;

	//Start and initialize engine
	SEngineParams params = {};
	params.ppCmdArgs	= nullptr;
	params.CmdArgsCount	= 0;
	return LambdaMain(params);
}

#endif
