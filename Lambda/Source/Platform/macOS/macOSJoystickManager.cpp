#include "LambdaPch.h"
#include "macOSJoystickManager.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    //---------------
    //JoystickManager
    //---------------
    
    GamePadManager* GamePadManager::Create()
    {
        return DBG_NEW macOSGamePadManager();
    }
    
    //--------------------
    //macOSGamePadManager
    //--------------------
    
    void macOSGamePadManager::InternalOnUpdate()
    {
        
    }
    
    
    void macOSGamePadManager::InternalSetPollrate(const Timestep& time)
    {
        
    }
    
    
    Timestep macOSGamePadManager::InternalGetPollrate() const
    {
        return Timestep();
    }
}

#endif
