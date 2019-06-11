#include "macOSLog.h"
#if defined(LAMBDA_PLAT_MACOS)
#include <string>
#include <cstdio>

namespace Lambda
{
    ILog* ILog::Create()
    {
        return DBG_NEW MacOSLog();
    }
    
    
    void MacOSLog::Print(LogSeverity severity, const char* pFormat, ...)
    {
        std::string message;
        if (severity == LOG_SEVERITY_INFO)
        {
            message = "[INFO]: ";
        }
        else if (severity == LOG_SEVERITY_WARNING)
        {
            message = "[WARNING]: ";
        }
        else if (severity == LOG_SEVERITY_ERROR)
        {
            message = "[ERROR]: ";
        }
        
        va_list args;
        va_start(args, pFormat);
        vprintf(pFormat, args);
        va_end(args);
    }
}

#endif
