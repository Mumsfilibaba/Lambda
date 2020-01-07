#include "LambdaPch.h"

#include "Core/Event/SystemEventDispatcher.h"
#include "Core/Engine/Console.h"

namespace Lambda
{
    //----------------------
    //CSystemEventDispatcher
    //----------------------

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    CSystemEventDispatcher::CSystemEventDispatcher()
        : m_EventListeners()
    {
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    bool CSystemEventDispatcher::DispatchEvent(const SSystemEvent& event)
    {
        for (auto pListener : m_EventListeners)
        {
            if (pListener->OnSystemEvent(event))
            {
                return true;
            }
        }

        return false;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CSystemEventDispatcher::AddEventListener(ISystemEventListener* pListener)
    {
        LAMBDA_ASSERT_PRINT(pListener, "pListener cannot be nullptr\n");

        //LOG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Added EventListener\n");
        m_EventListeners.emplace_back(pListener);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CSystemEventDispatcher::RemoveEventListener(ISystemEventListener* pListener)
    {
        LAMBDA_ASSERT_PRINT(pListener, "pListener cannot be nullptr\n");

        for (size_t i = 0; i < m_EventListeners.size(); i++)
        {
            if (m_EventListeners[i] == pListener)
            {
                //LOG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Removed EventListener\n");

                m_EventListeners.erase(m_EventListeners.begin() + i);
                return;
            }
        }
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CSystemEventDispatcher::Release()
    {
        delete this;
    }
}
