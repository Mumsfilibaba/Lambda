#include "LambdaPch.h"
#include "Core/Console.h"

#include <stdarg.h>

#if defined(LAMBDA_PLAT_WINDOWS)
    #include "Platform/Windows/WindowsConsole.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/macOSConsole.h"
#endif

namespace Lambda
{
    //--------
    //NConsole
    //--------
    namespace NConsole
    {
        bool Initialize()
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            return NWindowsConsole::Initialize();
#elif defined(LAMBDA_PLAT_MACOS)
            return NmacOSConsole::Initialize();
#else
            return false;
#endif
        }


        bool HasConsole()
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            return NWindowsConsole::HasConsole();
#elif defined(LAMBDA_PLAT_MACOS)
            return NmacOSConsole::HasConsole();
#else
            return false;
#endif
        }


        void Print(const char* pFormat, ...)
        {
            va_list args;
            va_start(args, pFormat);
#if defined(LAMBDA_PLAT_WINDOWS)
            NWindowsConsole::Print(pFormat, args);
#elif defined(LAMBDA_PLAT_MACOS)
            NmacOSConsole::Print(pFormat, args);
#endif
            va_end(args, pFormat);
        }


        void Reset()
        {
            SetTextColor(EConsoleColor::CONSOLE_COLOR_WHITE);
        }


        void Release()
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            NWindowsConsole::Release();
#elif defined(LAMBDA_PLAT_MACOS)
            NmacOSConsole::HasConsole();
#endif
        }


        void SetTextColor(EConsoleColor color)
        {
#if defined(LAMBDA_PLAT_WINDOWS)
            NWindowsConsole::SetTextColor(color);
#elif defined(LAMBDA_PLAT_MACOS)
            NmacOSConsole::SetTextColor(color);
#endif
        }
    }
}