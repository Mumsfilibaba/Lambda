#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
    //-------------
    //EConsoleColor
    //-------------
    enum class EConsoleColor : uint32
    {
        CONSOLE_COLOR_UNKNOWN = 0,
        CONSOLE_COLOR_WHITE   = 1,
        CONSOLE_COLOR_RED     = 2,
        CONSOLE_COLOR_GREEN   = 3,
        CONSOLE_COLOR_YELLOW  = 4,
    };

    //-------
    //Console
    //-------
    namespace Console
    {
        bool LAMBDA_API Initialize();
        bool LAMBDA_API HasConsole();
        void LAMBDA_API Print(const char* pFormat, ...);
        void LAMBDA_API Reset();
        void LAMBDA_API Release();

        void LAMBDA_API SetTextColor(EConsoleColor color);
    }
}
