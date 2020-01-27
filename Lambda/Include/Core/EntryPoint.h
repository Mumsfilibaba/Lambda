#pragma once
#include "LambdaCore.h"

#include "Core/Game.h"
#include "Core/EngineLoop.h"

#include "Platform/Platform.h"

namespace Lambda
{
    //Needs to be implemented by client
    extern CGame* CreateGame();
}

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
    
    //Set CreateGameFunc - This is to make both static and dynamic linking work
    _CreateGame = CreateGame;

    //Init systems that is needed during init
    g_EngineLoop.PreInit();
    
    //Init engine systems
	g_EngineLoop.Init();

    //Run engine
	g_EngineLoop.Start();
	while (g_EngineLoop.IsRunning())
		g_EngineLoop.Tick();

    //Release systems initialized in init
	g_EngineLoop.Release();
    
    //Release systems initialized during preint
    g_EngineLoop.PostRelease();
    
    Platform::Release();
	return 0;
}
