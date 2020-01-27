#pragma once
#include "LambdaCore.h"

#include <stdarg.h>

namespace Lambda
{
    enum class EConsoleColor
    {
        CONSOLE_COLOR_UNKNOWN   = 0,
        CONSOLE_COLOR_RED       = 1,
        CONSOLE_COLOR_GREEN     = 2,
        CONSOLE_COLOR_BLUE      = 3,
        CONSOLE_COLOR_YELLOW    = 4,
        CONSOLE_COLOR_WHITE     = 5,
    };

    class IConsoleOutput
    {
    public:
        LAMBDA_INTERFACE(IConsoleOutput);
        
        virtual void Show() = 0;
        virtual void Hide() = 0;
        
        virtual void SetTitle(const char* pTitle) = 0;
        virtual void SetTextColor(EConsoleColor color) = 0;
        
        virtual void Write(const char* pFormat, ...) = 0;
        virtual void WriteV(const char* pFormat, va_list args) = 0;
        
        virtual void Clear() = 0;
    };

    LAMBDA_API extern IConsoleOutput* g_pConsoleOutput;
}
