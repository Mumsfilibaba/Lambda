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
        //Nothing for now since titles are not supported
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::SetTextColor(EConsoleColor)
    {
        //Nothing for now since colors are not supported
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Print(const char* pFormat, va_list args)
    {
        NSString* pNSFormat = [NSString stringWithUTF8String:pFormat];
        NSLogv(pNSFormat, args);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::PrintLine(const char* pFormat, va_list args)
    {
        //NSLog already writes a new line so just use print for now
        CMacConsole::Print(pFormat, args);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Reset()
    {
        //Nothing for now since colors are not supported
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Clear()
    {
        //Nothing for now since NSLog does not seem to be cleared
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CMacConsole::Release()
    {
        delete this;
    }
}
#endif
