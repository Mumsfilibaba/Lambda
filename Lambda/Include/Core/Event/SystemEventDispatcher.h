#pragma once
#include "SSystemEvent.h"
#include "ISystemEventListener.h"

#include <vector>

namespace Lambda
{
    //----------------
    //CSystemEventDispatcher
    //----------------
    class LAMBDA_API CSystemEventDispatcher
    {
    public:
        CSystemEventDispatcher();
        ~CSystemEventDispatcher() = default;
        
        LAMBDA_NO_COPY(CSystemEventDispatcher);
        
        bool DispatchEvent(const SSystemEvent& event);
        void AddEventListener(ISystemEventListener* pListener);
        void RemoveEventListener(ISystemEventListener* pListener);
        void Release();
    private:
        std::vector<ISystemEventListener*> m_EventListeners;
    };
}
