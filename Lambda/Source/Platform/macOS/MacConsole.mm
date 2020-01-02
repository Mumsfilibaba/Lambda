#import "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #import "Platform/macOS/MacConsole.h"

    #import <stdarg.h>
    #import <Foundation/Foundation.h>

namespace Lambda
{
    //----------
    //MacConsole
    //----------

    /*//////////////////////////*/
    CmacOSConsole::CmacOSConsole()
        : IConsole()
    {
    }

    /*//////////////////////////////*/
    bool CmacOSConsole::InternalInit()
    {
        return true;
    }

    /*////////////////////////////////////////////*/
    void CmacOSConsole::SetTitle(const char*)
    {
        //Nothing for now
    }

    /*/////////////////////////////////////////////////*/
    void CmacOSConsole::SetTextColor(EConsoleColor)
    {
        //Nothing for now
    }

    /*///////////////////////////////////////////////*/
    void CmacOSConsole::Print(const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        CmacOSConsole::Print(pFormat, args);
        va_end(args);
    }

    /*////////////////////////////////////////////////////////*/
    void CmacOSConsole::Print(const char* pFormat, va_list args)
    {
        NSString* pNSFormat = [NSString stringWithUTF8String:pFormat];
        NSLogv(pNSFormat, args);
    }

    /*///////////////////////////////////////////////////*/
    void CmacOSConsole::PrintLine(const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        CmacOSConsole::PrintLine(pFormat, args);
        va_end(args);
    }

    /*////////////////////////////////////////////////////////////*/
    void CmacOSConsole::PrintLine(const char* pFormat, va_list args)
    {
        NSString* pNSFormat = [NSString stringWithUTF8String:pFormat];
        NSLogv(pNSFormat, args);
        NSLog(@"\n");
    }

    /*////////////////////////*/
    void CmacOSConsole::Reset()
    {
        //Nothing for now
    }
    
    /*///////////////////////*/
    void CmacOSConsole::Clear()
    {
        //Nothing for now
    }
    
    /*/////////////////////////*/
    void CmacOSConsole::Release()
    {
        delete this;
    }
}
#endif
