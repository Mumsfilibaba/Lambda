#pragma once
#include "LambdaCore.h"

#include "Core/EngineLoop.h"

#include "Platform/PlatformApplication.h"
#include "Platform/PlatformTime.h"

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
	Platform::Application::Init(hInstance);
#else
	Platform::Application::Init();
#endif

	Platform::Time::Nanoseconds();

	CEngineLoop* pEngineLoop = CEngineLoop::Create();	
	pEngineLoop->Init();

	pEngineLoop->Start();
	while (pEngineLoop->IsRunning())
		pEngineLoop->Tick();

	pEngineLoop->Release();
	return 0;
}