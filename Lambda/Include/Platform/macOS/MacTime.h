#pragma once
#include "Platform/Generic/GenericTime.h"

#ifdef LAMBDA_PLAT_MACOS

namespace Lambda
{
    class LAMBDA_API CMacTime : public CGenericTime
    {
    public:
        LAMBDA_STATIC_CLASS(CMacTime);

        static void Init();
        static uint64 Nanoseconds();
        static uint64 Ticks();
        static uint64 TicksPerSecond();
    };

    typedef CMacTime PlatformTime;
}
#endif
