#include "macOSJoystickManager.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    JoystickManager* JoystickManager::Create()
    {
        return DBG_NEW MacOSJoystickManager();
    }
    
    
    void MacOSJoystickManager::InternalOnUpdate()
    {
        
    }
    
    
    void MacOSJoystickManager::InternalSetPollrate(const Time& time)
    {
        
    }
    
    
    Time MacOSJoystickManager::InternalGetPollrate() const
    {
        return Time();
    }
}

#endif
