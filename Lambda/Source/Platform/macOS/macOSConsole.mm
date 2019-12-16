#import "LambdaPch.h"
#import "Core/Console.h"

namespace Lambda
{
    void InitializeConsole()
    {
    }
    
    
    void SetConsoleColor(EConsoleColor)
    {
    }
    
    
    void PrintConsole(const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        vprintf(pFormat, args);
        va_end(args);
    }
    
    
    void ReleaseConsole()
    {
    }
}
