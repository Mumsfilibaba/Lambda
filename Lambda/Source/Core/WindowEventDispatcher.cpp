#include "LambdaPch.h"
#include "Core/WindowEventDispatcher.h"
#include "Core/IWindowEventListener.h"

namespace Lambda
{
	//---------------------
	//WindowEventDispatcher
	//---------------------

	void WindowEventDispatcher::AddListener(IWindowEventListener* pWindowListener)
	{
		LAMBDA_ASSERT(pWindowListener != nullptr, "[LAMBDA ENGINE] pWindowListener cannot be nullptr");
		m_Listeners.emplace_back(pWindowListener);
	}
	
	
	void WindowEventDispatcher::Release()
	{
		delete this;
	}


	void WindowEventDispatcher::OnWindowResize(uint32 width, uint32 height)
	{
		for (auto pListener : m_Listeners)
			pListener->OnWindowResize(width, height);
	}
	
	
	void WindowEventDispatcher::OnWindowDestroy()
	{
		for (auto pListener : m_Listeners)
			pListener->OnWindowDestroy();
	}
}