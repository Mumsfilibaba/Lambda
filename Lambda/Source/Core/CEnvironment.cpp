#include "LambdaPch.h"
#include "Core/CEnvironment.h"
#include "Core/Event/IEventListener.h"
#include "Core/Input/CEventMouseController.h"
#include "Core/Input/CEventKeyboardController.h"
#include "Core/Input/IGamepadController.h"
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


	void CEnvironment::SetMouseController(IMouseController* pMouseController)
	{
		LAMBDA_ASSERT_PRINT(pMouseController != nullptr, "pMouseController cannot be nullptr");
		
		SafeDelete(m_pMouseController);
		m_pMouseController = pMouseController;
	}


	void CEnvironment::SetKeyboardController(IKeyboardController* pKeyboardController)
	{
		LAMBDA_ASSERT_PRINT(pKeyboardController != nullptr, "pKeyboardController cannot be nullptr");

		SafeDelete(m_pKeyboardController);
		m_pKeyboardController = pKeyboardController;
	}


	void CEnvironment::SetGamepadController(IGamepadController* pGamepadController)
	{
		LAMBDA_ASSERT_PRINT(pGamepadController != nullptr, "pGamepadController cannot be nullptr");

		SafeDelete(m_pGamepadController);
		m_pGamepadController = pGamepadController;
	}


	void CEnvironment::Initialize()
	{
		//Create default input controllers
		if (m_pKeyboardController == nullptr)
		{
			CEventKeyboardController* pKeyboardController = DBG_NEW CEventKeyboardController();
			SetKeyboardController(pKeyboardController);
			AddEventListener(pKeyboardController);
		}

		if (m_pMouseController == nullptr)
		{
			CDummyMouseController* pMouseController = DBG_NEW CDummyMouseController();
			SetMouseController(pMouseController);
		}

		if (m_pGamepadController == nullptr)
		{
			CDummyGamepadController* pGamepadController = DBG_NEW CDummyGamepadController();
			SetGamepadController(pGamepadController);
		}
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
				//LOG_ENVIRONMENT_INFO("Removed EventListener\n");

				m_EventListeners.erase(m_EventListeners.begin() + i);
				return;
			}
		}
	}
}
