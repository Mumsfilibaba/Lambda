#pragma once
#include "SEvent.h"
#include "Core/CSingleton.h"

namespace Lambda
{
    class IEventListener;

    //----------------
    //CEventDispatcher
    //----------------
    class LAMBDA_API CEventDispatcher : public CSingleton<CEventDispatcher>
    {
    public:
        CEventDispatcher();
        ~CEventDispatcher() = default;
        
        LAMBDA_NO_COPY(CEventDispatcher);
        
        bool DispatchEvent(const SEvent& event);
        void AddEventListener(IEventListener* pListener);
        void RemoveEventListener(IEventListener* pListener);
        void Release();
    private:
        std::vector<IEventListener*> m_EventListeners;
    };
}
