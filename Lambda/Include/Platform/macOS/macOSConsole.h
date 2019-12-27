#pragma once
#include "Core/Console.h"
#if defined(LAMBDA_PLAT_MACOS)
#include <stdarg.h>

namespace Lambda
{
    //-------------
    //NmacOSConsole
    //-------------
    namespace NmacOSConsole
    {
        bool Initialize();
        bool HasConsole();
        void Print(const char* pFormat, va_list args);
        void Release();

        void SetTextColor(EConsoleColor color);
    }
}
#endif