#pragma once
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/LEngine.h"
	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

extern LAMBDA_API Lambda::Layer* (*_CreateGameLayer)(void);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	using namespace Lambda;

	//Set function for createing gamelayer - Needed for DLL- and Static-Lib compilation
	_CreateGameLayer = CreateGameLayer;

	//Start and initialize engine
	LEngineParams params = {};
	params.ppCmdArgs		= nullptr;
	params.CmdArgsCount		= 0;
	return LambdaMain(params);
}

#endif
