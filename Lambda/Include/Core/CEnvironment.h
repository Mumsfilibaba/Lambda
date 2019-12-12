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
        virtual ~CEnvironment();

        LAMBDA_NO_COPY(CEnvironment);
        
		virtual void ProcessEvents() = 0;
		virtual void Release() = 0;
        virtual void PrintF(const char* pFormat, ...) = 0;

		virtual void Initialize();
        virtual void AddEventListener(IEventListener* pListener);
		virtual void RemoveEventListener(IEventListener* pListener);
        
        _forceinline IKeyboardController* GetKeyboardController() const
        {
            LAMBDA_ASSERT_PRINT(m_pKeyboardController != nullptr, "Environment has not created a KeyboardController\n");
            return m_pKeyboardController;
        }
        
        _forceinline IMouseController* GetMouseController() const
        {
			LAMBDA_ASSERT_PRINT(m_pMouseController != nullptr, "Environment has not created a MouseController\n");
            return m_pMouseController;
        }
        
        _forceinline IGamepadController* GetGamepadController() const
        {
			LAMBDA_ASSERT_PRINT(m_pGamepadController != nullptr, "Environment has not created a GamepadController\n");
            return m_pGamepadController;
        }
	protected:
        virtual bool OnEvent(const CEvent& event);

		void SetMouseController(IMouseController* pMouseController);
		void SetKeyboardController(IKeyboardController* pKeyboardController);
		void SetGamepadController(IGamepadController* pGamepadController);
	protected:
		std::vector<IEventListener*> m_EventListeners;
        IMouseController* m_pMouseController;
        IGamepadController* m_pGamepadController;
        IKeyboardController* m_pKeyboardController;
	public:
		static CEnvironment* Create();
	};
}
