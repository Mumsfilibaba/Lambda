#pragma once
#include "Core/CApplication.h"

#if defined(LAMBDA_PLAT_WINDOWS)
    #include "Platform/Windows/CWindowsWindow.h"

    #define WIN32_LEAN_AND_MEAN 1
    #include <Windows.h>

namespace Lambda
{
    //-------------------
    //CWindowsApplication
    //-------------------
    class CWindowsApplication final : public CApplication
    {
    public:
        CWindowsApplication(CEngine* pEngine);
        ~CWindowsApplication();

        virtual void ProcessEvents() override final;
        
        virtual IWindow* GetNativeWindow() const override final
        {
            return m_pWindow;
        }
    protected:
        virtual bool Initialize() override final;
    private:
        CEngine* m_pEngine;
        CWindowsWindow* m_pWindow;
    public:
        static LRESULT CALLBACK MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    };
}
#endif