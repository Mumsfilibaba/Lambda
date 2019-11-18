#pragma once
#include "LambdaCore.h"
#include "Utilities/Singleton.h"

namespace Lambda
{
	class IWindowEventListener;

	//---------------------
	//WindowEventDispatcher
	//---------------------

	class LAMBDA_API WindowEventDispatcher : public Singleton<WindowEventDispatcher>
	{
	public:
		WindowEventDispatcher() = default;
		~WindowEventDispatcher() = default;

		void AddListener(IWindowEventListener* pWindowListener);
		void Release();
		
		void OnWindowMove(uint32 x, uint32 y);
		void OnWindowFocusChanges(bool hasFocus);
		void OnWindowResize(uint32 width, uint32 height);
		void OnWindowClose();
	private:
		std::vector<IWindowEventListener*> m_Listeners;
	};
}