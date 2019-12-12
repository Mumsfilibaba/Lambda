#include "LambdaPch.h"
#include "Core/CEnvironment.h"
#include "Core/Event/IEventListener.h"
#include "Core/Input/CEventMouseController.h"
#include "Core/Input/CEventKeyboardController.h"
#include "Core/Input/CDummyGamepadController.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/Windows/CWindowsEnvironment.h"
#elif defined(LAMBDA_PLAT_MACOS)
    #include "../Platform/macOS/CmacOSEnvironment.h"
#endif

namespace Lambda
{
	//------------
	//CEnvironment
	//------------

	template<>
	CEnvironment* CSingleton<CEnvironment>::s_pInstance = nullptr;

	CEnvironment* CEnvironment::Create()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		return DBG_NEW CWindowsEnvironment();
#elif defined(LAMBDA_PLAT_MACOS)
        return DBG_NEW CmacOSEnvironment();
#endif
	}

    CEnvironment::CEnvironment()
        : m_EventListeners(),
        m_pKeyboardController(nullptr),
        m_pMouseController(nullptr),
        m_pGamepadController(nullptr)
    {
    }

	
	CEnvironment::~CEnvironment()
	{
		SafeDelete(m_pMouseController);
		SafeDelete(m_pGamepadController);
		SafeDelete(m_pKeyboardController);
	}


    bool CEnvironment::OnEvent(const CEvent& event)
    {
        for (auto pListener : m_EventListeners)
        {
            if (pListener->OnEvent(event))
                return true;
        }
        
        return false;
    }


	void CEnvironment::Init()
	{
		//Create default input controllers
		CEventMouseController* pMouseController = DBG_NEW CEventMouseController();
		m_pMouseController = pMouseController;
		AddEventListener(pMouseController);

		CEventKeyboardController* pKeyboardController = DBG_NEW CEventKeyboardController();
		m_pKeyboardController	= pKeyboardController;
		AddEventListener(pKeyboardController);

		m_pGamepadController = DBG_NEW CDummyGamepadController();
	}


	void CEnvironment::AddEventListener(IEventListener* pListener)
    {
        LAMBDA_ASSERT_PRINT(pListener != nullptr, "[LAMBDA ENGINE] pListener cannot be nullptr");

		LOG_ENVIRONMENT_INFO("Added EventListener\n");
        m_EventListeners.emplace_back(pListener);
    }
	
	
	void CEnvironment::RemoveEventListener(IEventListener* pListener)
	{
		LAMBDA_ASSERT_PRINT(pListener != nullptr, "[LAMBDA ENGINE] pListener cannot be nullptr");
		for (size_t i = 0; i < m_EventListeners.size(); i++)
		{
			if (m_EventListeners[i] == pListener)
			{
				LOG_ENVIRONMENT_INFO("Removed EventListener\n");

				m_EventListeners.erase(m_EventListeners.begin() + i);
				return;
			}
		}
	}
}
