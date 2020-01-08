#pragma once
#include "SSystemEvent.h"
#include "ISystemEventListener.h"

#include <vector>

namespace Lambda
{
    //----------------------
    //CSystemEventDispatcher
    //----------------------
    class LAMBDA_API CSystemEventDispatcher
    {
    public:
        CSystemEventDispatcher();
        ~CSystemEventDispatcher() = default;
        
        LAMBDA_NO_COPY(CSystemEventDispatcher);
        
        bool DispatchEvent(const SSystemEvent& event);
        
        void AddListener(ISystemEventListener* pListener);
        void RemoveListener(ISystemEventListener* pListener);
    private:
        std::vector<ISystemEventListener*> m_EventListeners;
    };
}
