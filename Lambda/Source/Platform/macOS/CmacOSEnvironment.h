#pragma once
#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/CEnvironment.h"
    #include "CmacOSWindow.h"

namespace Lambda
{
    class CmacOSEnvironment : public CEnvironment
    {
    public:
        CmacOSEnvironment();
        ~CmacOSEnvironment();

        virtual void Init() override final;
        virtual void ProcessEvents() override final;
        virtual void Release() override final;
        virtual void PrintF(const char* pFormat, ...) override final;
    private:
        CmacOSWindow* m_pWindow;
    };
}
#endif
