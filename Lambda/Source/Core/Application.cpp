#include "LambdaPch.h"
#include "Core/Application.h"
#if defined(LAMBDA_PLAT_WINDOWS)
    #include "Platform/Windows/WindowsApplication.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/macOSApplication.h"
#else
    #error No platform defined
#endif

namespace Lambda
{
    //-----------
    //Application
    //-----------
    namespace Application
    {
        bool Initialize()
        {
            #if defined(LAMBDA_PLAT_WINDOWS)
                return WindowsApplication::Initialize();
            #elif defined(LAMBDA_PLAT_MACOS)
                return macOSApplication::Initialize();
            #else
                return false;
            #endif
        }


        void ProcessEvents()
        {
            #if defined(LAMBDA_PLAT_WINDOWS)
                WindowsApplication::ProcessEvents();
            #elif defined(LAMBDA_PLAT_MACOS)
                macOSApplication::ProcessEvents();
            #else
            #endif
        }


        void Release()
        {
            #if defined(LAMBDA_PLAT_WINDOWS)
                WindowsApplication::Release();
            #elif defined(LAMBDA_PLAT_MACOS)
                macOSApplication::Release();
            #else
            #endif
        }


        void SetEventCallback(const CEventCallback& eventCallback)
        {
            #if defined(LAMBDA_PLAT_WINDOWS)
                WindowsApplication::SetEventCallback(eventCallback);
            #elif defined(LAMBDA_PLAT_MACOS)
                macOSApplication::SetEventCallback(eventCallback);
            #else
            #endif
        }


        IWindow* GetNativeWindow()
        {
            #if defined(LAMBDA_PLAT_WINDOWS)
                return WindowsApplication::GetNativeWindow();
            #elif defined(LAMBDA_PLAT_MACOS)
                return macOSApplication::GetNativeWindow();
            #else
                return nullptr;
            #endif
        }
    }
}
