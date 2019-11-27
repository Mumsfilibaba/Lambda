#include "LambdaPch.h"
#include "Core/WindowEventDispatcher.h"
#include "Core/IWindowEventListener.h"

namespace Lambda
{
	//---------------------
	//WindowEventDispatcher
	//---------------------

	template<>
	WindowEventDispatcher* Singleton<WindowEventDispatcher>::s_pInstance = nullptr;

	void WindowEventDispatcher::AddListener(IWindowEventListener* pWindowListener)
	{
		LAMBDA_ASSERT_PRINT(pWindowListener != nullptr, "[LAMBDA ENGINE] pWindowListener cannot be nullptr");
		s_Listeners.emplace_back(pWindowListener);
	}
	
	
	void WindowEventDispatcher::Release()
	{
		delete this;
	}


	void WindowEventDispatcher::OnWindowMove(uint32 x, uint32 y)
	{
		for (auto pListener : m_Listeners)
			pListener->OnWindowMove(x, y);
	}


	void WindowEventDispatcher::OnWindowFocusChanges(bool hasFocus)
	{
		for (auto pListener : m_Listeners)
			pListener->OnWindowFocusChanges(hasFocus);
	}


	void WindowEventDispatcher::OnWindowResize(uint32 width, uint32 height)
	{
		for (auto pListener : m_Listeners)
			pListener->OnWindowResize(width, height);
	}
	
	
	void WindowEventDispatcher::OnWindowClose()
	{
		for (auto pListener : m_Listeners)
			pListener->OnWindowClose();
	}
}