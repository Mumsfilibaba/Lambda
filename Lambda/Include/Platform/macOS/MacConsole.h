#pragma once
#include "Core/Engine/Console.h"

namespace Lambda
{
    //-----------
    //CMacConsole
    //-----------
    class CMacConsole final : public IConsole
    {
    public:
        CMacConsole();
        ~CMacConsole() = default;

        virtual void SetTitle(const char* pTitle) override final;
		virtual void SetTextColor(EConsoleColor color) override final;
        
        virtual void Print(const char* pFormat, ...) override final;
		virtual void PrintV(const char* pFormat, va_list args) override final;
		
        virtual void PrintLine(const char* pFormat, ...) override final;
        virtual void PrintLineV(const char* pFormat, va_list args) override final;

		virtual void Reset() override final;
		virtual void Clear() override final;
		
		virtual void Release() override final;
    };
}
