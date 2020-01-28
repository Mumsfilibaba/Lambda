#pragma once
#include "Core/Debug/IConsoleOutput.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "CococaConsoleWindow.h"

namespace Lambda
{
    class CMacConsoleOutput final : public IConsoleOutput
    {
    public:
        CMacConsoleOutput();
        ~CMacConsoleOutput();
        
        virtual void Show() override final;
        virtual void Hide() override final;
        
        virtual void SetTitle(const char* pTitle) override final;
        virtual void SetTextColor(EConsoleColor color) override final;
        
        virtual void Write(const char* pFormat, ...) override final;
        virtual void WriteV(const char* pFormat, va_list args) override final;
        
        virtual void Clear() override final;
    private:
        NSTextView* m_pTextView;
        NSScrollView* m_pScrollView;
        NSDictionary* m_pTextViewTextColor;
        CocoaConsoleWindow* m_pConsoleWindow;
    };
}
#endif
