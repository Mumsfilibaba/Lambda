#pragma once
#include "Core/Engine/Engine.h"

#if !defined(WIN32_LEAN_AND_MEAN)
	#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>

extern LAMBDA_API Lambda::Layer* (*_CreateGameLayer)();

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	using namespace Lambda;

	//Set function for createing gamelayer - Needed for DLL- and Static-Lib compilation
	_CreateGameLayer = CreateGameLayer;

	//Start and initialize engine
	EngineParams params = {};
	params.ppCmdLineArgs = nullptr;
	params.CmdLineArgsCount = 0;
	return LambdaMain(params);
}