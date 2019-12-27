#pragma once
#include "Core/Console.h"

namespace Lambda
{
    //---------------
    //NWindowsConsole
    //---------------
    namespace NWindowsConsole
    {
        bool Initialize();
        bool HasConsole();
        void Print(const char* pFormat, va_list args);
        void Release();

        void SetTextColor(EConsoleColor color);
    }
}