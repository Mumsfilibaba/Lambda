#pragma once
#include "LambdaCore.h"
#if defined(LAMBDA_PLAT_WINDOWS)
    #define WIN32_LEAN_AND_MEAN 1
    #include <Windows.h>
namespace Lambda
{
    //------------------
    //WindowsApplication
    //------------------
    namespace WindowsApplication
    {
        bool Initialize();
        void ProcessEvents();
        void Release();

        void SetEventCallback(const CEventCallback& eventCallback);

        IWindow* GetNativeWindow();

        LRESULT CALLBACK MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    }
}