#pragma once
#include "LambdaCore.h"
#include "CSingleton.h"

namespace Lambda
{
    class CEvent;
    class IEventListener;
    class IKeyboardController;
    class IMouseController;
    class IGamepadController;

    //------------
	//CEnvironment
	//------------

	class CEnvironment : public CSingleton<CEnvironment>
	{
	public:
        CEnvironment();
        virtual ~CEnvironment() = default;

        LAMBDA_NO_COPY(CEnvironment);
        
		virtual void Init() = 0;
		virtual void ProcessEvents() = 0;
		virtual void Release() = 0;
        virtual void PrintF(const char* pFormat, ...) = 0;
        virtual void AddEventListener(IEventListener* pListener);
        
        _forceinline IKeyboardController* GetKeyboardController() const
        {
            LAMBDA_ASSERT(m_pKeyboardController != nullptr);
            return m_pKeyboardController;
        }
        
        _forceinline IMouseController* GetMouseController() const
        {
            LAMBDA_ASSERT(m_pMouseController != nullptr);
            return m_pMouseController;
        }
        
        _forceinline IGamepadController* GetGamepadController() const
        {
            LAMBDA_ASSERT(m_pGamepadController != nullptr);
            return m_pGamepadController;
        }
	protected:
        virtual void OnEvent(const CEvent& event);
	protected:
		std::vector<IEventListener*> m_EventListeners;
        IKeyboardController* m_pKeyboardController;
        IMouseController* m_pMouseController;
        IGamepadController* m_pGamepadController;
	public:
		static CEnvironment* Create();
	};
}
