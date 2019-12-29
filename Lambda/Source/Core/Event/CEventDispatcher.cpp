#include "LambdaPch.h"
#include "Core/Event/CEventDispatcher.h"
#include "Core/Event/IEventListener.h"
#include "Core/CLogManager.h"

namespace Lambda
{
    //----------------
    //CEventDispatcher
    //----------------

    template<>
    CEventDispatcher* CSingleton<CEventDispatcher>::s_pInstance = nullptr;

    CEventDispatcher::CEventDispatcher()
        : CSingleton(),
        m_EventListeners()
    {
    }


    bool CEventDispatcher::DispatchEvent(const SSystemEvent& event)
    {
        for (auto pListener : m_EventListeners)
        {
            if (pListener->OnEvent(event))
                return true;
        }

        return false;
    }


    void CEventDispatcher::AddEventListener(IEventListener* pListener)
    {
        LAMBDA_ASSERT_PRINT(pListener != nullptr, "pListener cannot be nullptr\n");

        LOG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Added EventListener\n");
        m_EventListeners.emplace_back(pListener);
    }


    void CEventDispatcher::RemoveEventListener(IEventListener* pListener)
    {
        LAMBDA_ASSERT_PRINT(pListener != nullptr, "pListener cannot be nullptr\n");

        for (size_t i = 0; i < m_EventListeners.size(); i++)
        {
            if (m_EventListeners[i] == pListener)
            {
                LOG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Removed EventListener\n");

                m_EventListeners.erase(m_EventListeners.begin() + i);
                return;
            }
        }
    }
    
    
    void CEventDispatcher::Release()
    {
        delete this;
    }
}