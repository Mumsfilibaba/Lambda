#pragma once
#include "System/Log.h"

#if defined(LAMBDA_PLAT_MACOS)
namespace Lambda
{
    //--------
    //macOSLog
    //--------
    
    class macOSLog final : public ILog
    {
    public:
        LAMBDA_NO_COPY(macOSLog);
        
        macOSLog() = default;
        virtual ~macOSLog() = default;
        
        virtual void Print(LogSeverity severity, const char* pFormat, ...) override final;
    };
}
#endif
