#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/macOS/MacTime.h"

namespace Lambda
{
    void CMacTime::Init()
    {
    }

    uint64 CMacTime::Nanoseconds()
    {
        return 0;
    }

    uint64 CMacTime::Ticks()
    {
        return 0;
    }

    uint64 CMacTime::TicksPerSecond()
    {
        return 0;
    }
}
#endif
