#pragma once
#include "Core/Input/CMouseControllerBase.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    class CEnvironment;

    //---------------------
    //CmacOSMouseController
    //---------------------

    class CmacOSMouseController final : public CMouseControllerBase
    {
    public:
        CmacOSMouseController(CEnvironment* pEnvironment);
        ~CmacOSMouseController() = default;
        
        virtual bool IsMouseVisible() const override final;
        virtual void SetMouseVisisble(bool) override final;
        virtual void SetPosition(const Point& position) override final;
        virtual Point GetPosition() const override final;
    };
}

#endif
