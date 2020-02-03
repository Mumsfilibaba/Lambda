#pragma once
#include "LambdaCore.h"

#include "Core/Game.h"
#include "Core/Engine.h"

#include "Platform/Platform.h"

namespace Lambda
{
    //Needs to be implemented by client
    extern CGame* CreateGameInstance();
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

    _CreateGameInstance = CreateGameInstance;
    
    //Init systems that is needed during init
    g_Engine.PreInit();
    
    //Init engine systems
    g_Engine.Init();

    //Run engine
    g_Engine.Run();

    //Release systems initialized in init
    g_Engine.Release();
    
    //Release systems initialized during preint
    g_Engine.PostRelease();
    
    Platform::Release();
	return 0;
}
