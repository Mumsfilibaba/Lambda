#pragma once
#include "Core/Engine/Engine.h"

#include "WindowsApplication.h"

#if !defined(WIN32_LEAN_AND_MEAN)
	#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>

extern LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	using namespace Lambda;

	CWindowsApplication::PreInit(hInstance);

	//Set function for createing gamelayer - Needed for DLL- and Static-Lib compilation
	_CreateGameLayer = CreateGameLayer;

	//Start and initialize engine
	SEngineParams params = {};
	params.ppCmdLineArgs = nullptr;
	params.CmdLineArgsCount = 0;
	return LambdaMain(params);
}