#pragma once
#include "Core/Engine/Console.h"

#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    //----------
    //MacConsole
    //----------

    class MacConsole final : public IConsole
    {
    public:
        MacConsole();
        ~MacConsole() = default;

        virtual void SetTitle(const char* pTitle) override final;
		virtual void SetTextColor(EConsoleColor color) override final;
        
        virtual void Print(const char* pFormat, ...) override final;
		virtual void PrintV(const char* pFormat, va_list args) override final;
		
        virtual void PrintLine(const char* pFormat, ...) override final;
        virtual void PrintLineV(const char* pFormat, va_list args) override final;

		virtual void Reset() override final;
		virtual void Clear() override final;
    };
}
#endif
