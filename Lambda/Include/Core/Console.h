#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    //-------------
    //EConsoleColor
    //-------------

    enum EConsoleColor : uint8
    {
        CONSOLE_COLOR_UNKNOWN   = 0,
        CONSOLE_COLOR_RED       = 1,
        CONSOLE_COLOR_BLUE      = 2,
        CONSOLE_COLOR_GREEN     = 3,
        CONSOLE_COLOR_YELLOW    = 4,
        CONSOLE_COLOR_WHITE     = 5,
    };

    //---------------------
    //Interface for console
    //---------------------

    void InitializeConsole();
    void SetConsoleColor(EConsoleColor color);
    void PrintConsole(const char* pFormat, ...);
    void ReleaseConsole();
}
