#import "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #import "Platform/macOS/MacConsole.h"

    #import <stdarg.h>
    #import <Foundation/Foundation.h>

namespace Lambda
{
    //-----------
    //CMacConsole
    //-----------

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    CMacConsole::CMacConsole()
        : IConsole()
    {
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    bool CMacConsole::Initialize()
    {
        return true;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::SetTitle(const char*)
    {
        //Nothing for now
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::SetTextColor(EConsoleColor)
    {
        //Nothing for now
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Print(const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        CMacConsole::Print(pFormat, args);
        va_end(args);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Print(const char* pFormat, va_list args)
    {
        NSString* pNSFormat = [NSString stringWithUTF8String:pFormat];
        NSLogv(pNSFormat, args);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::PrintLine(const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        CMacConsole::PrintLine(pFormat, args);
        va_end(args);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::PrintLine(const char* pFormat, va_list args)
    {
        NSString* pNSFormat = [NSString stringWithUTF8String:pFormat];
        NSLogv(pNSFormat, args);
        NSLog(@"\n");
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Reset()
    {
        //Nothing for now
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Clear()
    {
        //Nothing for now
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Release()
    {
        delete this;
    }
}
#endif
