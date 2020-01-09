#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Engine/System.h"
	#include "Core/Event/SystemEventDispatcher.h"

	#include "Platform/Windows/WindowsWindow.h"

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	//-------------
	//WindowsSystem
	//-------------

	class WindowsSystem final : public ISystem
	{
	public:
		/*ISystem Interface*/
		virtual IWindow* CreateWindow(const char* pTitle, uint32 width, uint32 height) override final;
		virtual void ProcessEvents() override final;

		virtual void AddListener(ISystemEventListener* pListener) override final	
		{
			m_EventDispatcher.AddListener(pListener); 
		}
		
		virtual void RemoveListener(ISystemEventListener* pListener) override final 
		{ 
			m_EventDispatcher.RemoveListener(pListener);
		}

		/*WindowsSystem interface*/
		LRESULT OnMessage(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam);

		void DispatchEvent(const SystemEvent& event) 
		{ 
			m_EventDispatcher.DispatchEvent(event); 
		}
				
		HINSTANCE GetHINSTANCE() const 
		{ 
			return m_hInstance; 
		}
	private:
		WindowsSystem();
		~WindowsSystem();
	private:
		SystemEventDispatcher m_EventDispatcher;
		HINSTANCE m_hInstance;
	public:
		static ISystem* Create();
		static void RegisterWindowClass(HINSTANCE hInstance);
	};
}
#endif