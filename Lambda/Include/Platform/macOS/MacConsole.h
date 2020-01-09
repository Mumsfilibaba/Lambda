#pragma once
#include "Core/Engine/Console.h"

#if defined(LAMBDA_PLAT_MACOS)

#if defined(__OBJC__)
    #include <Cocoa/Cocoa.h>

    //------------------
    //CocoaConsoleWindow
    //------------------
    @interface CocoaConsoleWindow : NSWindow<NSWindowDelegate>
    {
    }
    @end
#else
    class NSTextView;
    class NSDictionary;
    class CocoaConsoleWindow;
#endif

namespace Lambda
{
    //----------
    //MacConsole
    //----------

    class MacConsole final : public IConsole
    {
    public:
        MacConsole();
        ~MacConsole();

        virtual void SetTitle(const char* pTitle) override final;
		virtual void SetTextColor(EConsoleColor color) override final;
        
        virtual void Print(const char* pFormat, ...) override final;
		virtual void PrintV(const char* pFormat, va_list args) override final;
		
        virtual void PrintLine(const char* pFormat, ...) override final;
        virtual void PrintLineV(const char* pFormat, va_list args) override final;

		virtual void Reset() override final;
		virtual void Clear() override final;
    private:
        NSTextView* m_pTextView;
        NSDictionary* m_pTextViewTextColor;
        CocoaConsoleWindow* m_pConsoleWindow;
    };
}
#endif
