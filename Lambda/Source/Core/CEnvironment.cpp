#include "LambdaPch.h"
#include "Core/CEnvironment.h"
#include "Core/Event/IEventListener.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Platform/Windows/WindowsEnvironment.h"
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
		return DBG_NEW WindowsEnvironment();
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


    bool CEnvironment::OnEvent(const CEvent& event)
    {
        for (auto pListener : m_EventListeners)
        {
            if (pListener->OnEvent(event))
                return true;
        }
        
        return false;
    }


    void CEnvironment::AddEventListener(IEventListener* pListener)
    {
        LAMBDA_ASSERT_PRINT(pListener != nullptr, "[LAMBDA ENGINE] pListener cannot be nullptr");
        m_EventListeners.emplace_back(pListener);
    }
}
