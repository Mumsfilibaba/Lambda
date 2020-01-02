#pragma once
#include "Core/Engine/Console.h"

namespace Lambda
{
    //----------
    //MacConsole
    //----------
    class CmacOSConsole final : public IConsole
    {
    public:
        CmacOSConsole();
        ~CmacOSConsole() = default;

        virtual void SetTitle(const char* pTitle) override final;
		virtual void SetTextColor(EConsoleColor color) override final;

		virtual void Print(const char* pFormat, ...) override final;
		virtual void Print(const char* pFormat, va_list args) override final;
		
		virtual void PrintLine(const char* pFormat, ...) override final;
		virtual void PrintLine(const char* pFormat, va_list args) override final;

		virtual void Reset() override final;
		virtual void Clear() override final;
		
		virtual void Release() override final;
    private:
        virtual bool InternalInit() override final;
    };
}
