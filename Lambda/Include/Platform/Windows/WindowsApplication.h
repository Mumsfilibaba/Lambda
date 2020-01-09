#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Engine/Application.h"
	#include "Core/Event/SystemEventDispatcher.h"

	#include "Platform/Windows/WindowsWindow.h"

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	//-------------------
	//CWindowsApplication
	//-------------------
	class CWindowsApplication final : public IApplication
	{
	public:
		/*IApplication Interface*/
		virtual void Release() override final;
		
		virtual bool CreateWindow(const char* pTitle, uint32 width, uint32 height) override final;

		virtual void ProcessEvents() override final;
		virtual void AddListener(ISystemEventListener* pListener) override final	{ m_EventDispatcher.AddListener(pListener); }
		virtual void RemoveListener(ISystemEventListener* pListener) override final { m_EventDispatcher.RemoveListener(pListener); }

		virtual const IWindow* GetWindow() const override final { return m_pWindow; }	

		/*CWindowsApplication interface*/
		void DispatchEvent(const SSystemEvent& event) { m_EventDispatcher.DispatchEvent(event); }
		
		void OnWindowClose(CWindowsWindow* pWindow);
		LRESULT OnMessage(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam);
		
		/*////////////////////////////////////////////////////////////////////////////////////////////////*/
		HINSTANCE GetHINSTANCE() const 
		{ 
			return m_hInstance; 
		}
	private:
		CWindowsApplication();
		~CWindowsApplication();
	private:
		CWindowsWindow* m_pWindow;
		CSystemEventDispatcher m_EventDispatcher;

		HINSTANCE m_hInstance;
	public:
		static IApplication* CreateWindowsApplication();
		static void RegisterWindowClass(HINSTANCE hInstance);
	};
}
#endif