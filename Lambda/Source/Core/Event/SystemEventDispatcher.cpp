#include "LambdaPch.h"

#include "Core/Event/SystemEventDispatcher.h"
#include "Core/Engine/Console.h"

namespace Lambda
{
    //----------------------
    //SystemEventDispatcher
    //----------------------

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    SystemEventDispatcher::SystemEventDispatcher()
        : m_EventListeners()
    {
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    bool SystemEventDispatcher::DispatchEvent(const SystemEvent& event)
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
    void SystemEventDispatcher::AddListener(ISystemEventListener* pListener)
    {
        LAMBDA_ASSERT_PRINT(pListener, "pListener cannot be nullptr\n");

        //LOG(LOG_CHANNEL_ALL_CHANNELS, LOG_SEVERITY_INFO, "Added EventListener\n");
        m_EventListeners.emplace_back(pListener);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void SystemEventDispatcher::RemoveListener(ISystemEventListener* pListener)
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
}
