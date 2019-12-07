#pragma once
#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Environment.h"

namespace Lambda
{
    class macOSEnvironment : public Environment
    {
    public:
        macOSEnvironment() = default;
        ~macOSEnvironment() = default;

        virtual void Init() override final;
        virtual void ProcessEvents() override final;
        virtual void Release() override final;
        virtual void PrintF(const char* pFormat, ...) override final;

        //Window Events
        virtual void OnWindowMove(uint32 x, uint32 y) override final;
        virtual void OnWindowFocusChanges(bool hasFocus) override final;
        virtual void OnWindowResize(uint32 width, uint32 height) override final;
        virtual void OnWindowClose() override final;
    };
}
#endif
