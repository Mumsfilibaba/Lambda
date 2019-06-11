#pragma once
#include "System/JoystickManager.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    class MacOSJoystickManager final : public JoystickManager
    {
    public:
        LAMBDA_NO_COPY(MacOSJoystickManager);
        
        MacOSJoystickManager() = default;
        ~MacOSJoystickManager() = default;
        
        virtual void InternalOnUpdate() override final;
        virtual void InternalSetPollrate(const Time& time) override final;
        virtual Time InternalGetPollrate() const override final;
    };
}
#endif
