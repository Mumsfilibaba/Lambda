#include "LambdaCore.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/Common/CommonTime.h"

    #include <mach/mach_time.h>

namespace Lambda
{
    class MacTime final : public CommonTime
    {
    public:
        LAMBDA_DECL_STATIC_CLASS(MacTime);
        
        _forceinline static void Init()
        {
            //Init frequency
            mach_timebase_info_data_t timeData = {};
            mach_timebase_info(&timeData);
            s_NanoSecTickFactor = (uint64)timeData.numer / (uint64)timeData.denom;
        }
        
        _forceinline static uint64 Nanoseconds()
        {
            return Ticks() * s_NanoSecTickFactor;
        }

        _forceinline static uint64 Ticks()
        {
            return (uint64)mach_absolute_time();
        }

        _forceinline static uint64 TicksPerSecond()
        {
            //Convert to seconds - In nanoseconds on macOS, since this is based on the
            //windows version we need to convert to seconds since WinAPI returns in seconds
            return (s_NanoSecTickFactor * 1000000000UL);
        }
    private:
        inline static uint64 s_NanoSecTickFactor = { 0 };
    };

    typedef MacTime PlatformTime;
}
#endif
