#pragma once
#include "System/GamePadManager.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    //-------------------
    //macOSGamePadManager
    //-------------------
    
    class macOSGamePadManager final : public GamePadManager
    {
    public:
        LAMBDA_NO_COPY(macOSGamePadManager);
        
        macOSGamePadManager() = default;
        ~macOSGamePadManager() = default;
        
        virtual void InternalOnUpdate() override final;
        virtual void InternalSetPollrate(const Timestep& time) override final;
        virtual Timestep InternalGetPollrate() const override final;
    };
}
#endif
