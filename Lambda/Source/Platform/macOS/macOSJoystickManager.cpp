#include "LambdaPch.h"
#include "macOSJoystickManager.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    //---------------
    //JoystickManager
    //---------------
    
    JoystickManager* JoystickManager::Create()
    {
        return DBG_NEW MacOSJoystickManager();
    }
    
    //--------------------
    //MacOSJoystickManager
    //--------------------
    
    void MacOSJoystickManager::InternalOnUpdate()
    {
        
    }
    
    
    void MacOSJoystickManager::InternalSetPollrate(const Timestep& time)
    {
        
    }
    
    
    Timestep MacOSJoystickManager::InternalGetPollrate() const
    {
        return Timestep();
    }
}

#endif
