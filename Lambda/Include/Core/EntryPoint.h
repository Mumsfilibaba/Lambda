#pragma once
#include "LambdaCore.h"

#include "Core/EngineLoop.h"
#include "Core/Log/EngineLog.h"

#include "Platform/Platform.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>
#endif

#ifdef LAMBDA_PLAT_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
#else
int main(int, const char**)
#endif
{
	using namespace Lambda;

#ifdef LAMBDA_PLAT_WINDOWS
	Platform::Init(hInstance);
#else
	Platform::Init();
#endif

	CEngineLog::Init();

	g_EngineLoop.Init();

	g_EngineLoop.Start();
	while (g_EngineLoop.IsRunning())
		g_EngineLoop.Tick();

	g_EngineLoop.Release();

	CEngineLog::Release();
	return 0;
}
