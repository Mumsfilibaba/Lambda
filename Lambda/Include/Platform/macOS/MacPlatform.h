#pragma once
#include "Platform/Generic/GenericPlatform.h"

#ifdef LAMBDA_PLAT_MACOS

namespace Lambda
{
    class LAMBDA_API CMacPlatform final : public CGenericPlatform
    {
    public:
        LAMBDA_STATIC_CLASS(CMacPlatform);

        static void Init();
        static void CreateMenuBar();
        
        static void PollEvents();
        
        static void MessageBox(const char* pCaption, const char* pText, uint32 type);
        static void OutputDebugString(const char* pMessage);
        
        static uint64 Nanoseconds();
        static uint64 Ticks();
        static uint64 TicksPerSecond();
    private:
        static uint64 s_NanoSecTickFactor;
    };

    typedef CMacPlatform Platform;
}
#endif
