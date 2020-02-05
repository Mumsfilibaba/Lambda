#pragma once
#include "Platform/Common/CommonPlatform.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Core/Input/Mouse.h"

    #include "MacAppDelegate.h"

namespace Lambda
{
    class LAMBDA_API MacPlatform final : public CommonPlatform
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(MacPlatform);

        static IConsoleOutput* CreateConsoleOutput();
        static IWindow* CreateWindow(const SWindowDesc& desc);
        static void CreateMenuBar();
        
        static void Init();
        static void Release();
        
        static void PollEvents();
        
        static void MessageBox(const char* pCaption, const char* pText, uint32 type);
        static void OutputDebugString(const char* pMessage);
        
        static EMouseButton ConvertMouseButton(uint32 button);
    private:
        static CMacAppDelegate* s_pAppDelegate;
    };

    typedef MacPlatform Platform;
}
#endif
