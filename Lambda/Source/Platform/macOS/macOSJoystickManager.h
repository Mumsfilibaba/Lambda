#pragma once
#include "System/JoystickManager.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    //--------------------
    //MacOSJoystickManager
    //--------------------
    
    class MacOSJoystickManager final : public JoystickManager
    {
    public:
        LAMBDA_NO_COPY(MacOSJoystickManager);
        
        MacOSJoystickManager() = default;
        ~MacOSJoystickManager() = default;
        
        virtual void InternalOnUpdate() override final;
        virtual void InternalSetPollrate(const Timestep& time) override final;
        virtual Timestep InternalGetPollrate() const override final;
    };
}
#endif
