#import "LambdaPch.h"
#if defined(LAMBDA_PLAT_MACOS)
    #import "Platform/macOS/Console.h"
    #import <stdio.h>

namespace Lambda
{
    bool Initialize()
    {
        //On macos we do not have to initialize anything
        return true;
    }


    bool HasConsole()
    {
        //We always return false for now, since we do not open a console, and rely on if the application was started from the terminal
        return false;
    }


    void Print(const char* pFormat, va_list args)
    {
        vprintf(pFormat, args);
    }


    void Release()
    {
        //Nothing to release for now
    }


    void SetTextColor(EConsoleColor color)
    {
        //We do not set color for now
    }
}
#endif
