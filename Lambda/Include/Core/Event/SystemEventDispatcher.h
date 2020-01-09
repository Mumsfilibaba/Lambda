#pragma once
#include "SystemEvent.h"
#include "ISystemEventListener.h"

#include <vector>

namespace Lambda
{
    //---------------------
    //SystemEventDispatcher
    //---------------------

    class LAMBDA_API SystemEventDispatcher
    {
    public:
        SystemEventDispatcher();
        ~SystemEventDispatcher() = default;
        
        LAMBDA_NO_COPY(SystemEventDispatcher);
        
        bool DispatchEvent(const SystemEvent& event);
        
        void AddListener(ISystemEventListener* pListener);
        void RemoveListener(ISystemEventListener* pListener);
    private:
        std::vector<ISystemEventListener*> m_EventListeners;
    };
}
