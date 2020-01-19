#pragma once
#include "Platform/Application.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include <vector>
	#include <unordered_map>
	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>
	#if defined (CreateWindow)
		#undef CreateWindow
	#endif

namespace Lambda
{
	class CWindowsWindow;

	//-------------------
	//CWindowsApplication
	//-------------------

	class LAMBDA_API CWindowsApplication : public CApplication
	{
	private:
		CWindowsApplication();
	public:
		~CWindowsApplication();

		CWindowsWindow* FindWindowFromHWND(HWND hWnd) const;
		
		/*Lets the application instance handle the message*/
		LRESULT HandleMessage(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam);

		/*Register class for window*/
		static void RegisterWindowClass(HINSTANCE hInstance);
		
		/*Window procedure, handles all native events*/
		static LRESULT CALLBACK WndProc(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam);
		
		static HINSTANCE GetApplicationInstance() { return s_hInstance; }
	public:
		/*CApplication interface*/
		virtual CWindow* CreateWindow(const SWindowProps& props) override final;
		virtual CWindow* GetCurrentWindow() const override final;

		virtual void Terminate(int32 exitCode) const override final;

		/*Windows specific preinit, takes in the HINSTANCE from WinMain*/
		static void PreInit(HINSTANCE hInstance);

		static void Init();
		static void ProcessEvents();
		static void Release();

		static CApplication* CreateApplication();
	private:
		std::unordered_map<HWND, CWindowsWindow*> m_Windows;

		static HINSTANCE s_hInstance;
		static CWindowsApplication* s_pWindowsApplication;
	};

	//--------------------
	//CPlatformApplication
	//--------------------

	typedef CWindowsApplication CPlatformApplication;
}
#endif