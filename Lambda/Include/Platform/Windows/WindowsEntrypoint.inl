#pragma once
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Engine/Engine.h"

	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

extern LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)(Lambda::IEngine*);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	using namespace Lambda;

	//Set function for createing gamelayer - Needed for DLL- and Static-Lib compilation
	_CreateGameLayer = CreateGameLayer;

	//Start and initialize engine
	SEngineParams params = {};
	params.ppCmdLineArgs	 = nullptr;
	params.nCmdLineArgsCount = 0;
	return LambdaMain(params);
}

#endif