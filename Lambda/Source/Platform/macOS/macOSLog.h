#pragma once
#include "System/Log.h"

#if defined(LAMBDA_PLAT_MACOS)
namespace Lambda
{
    //--------
    //MacOSLog
    //--------
    
    class MacOSLog final : public ILog
    {
    public:
        LAMBDA_NO_COPY(MacOSLog);
        
        MacOSLog() = default;
        virtual ~MacOSLog() = default;
        
        virtual void Print(LogSeverity severity, const char* pFormat, ...) override final;
    };
}
#endif
