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
        
		virtual void Print(const char* pFormat, va_list args) override final;
		virtual void PrintLine(const char* pFormat, va_list args) override final;

		virtual void Reset() override final;
		virtual void Clear() override final;
		
		virtual void Release() override final;
    private:
        virtual bool Initialize() override final;
    };
}
