#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "CWindowsEnvironment.h"
	#include "CWindowsWindow.h"
	#include "Core/Event/CEventDispatcher.h"
	#include "Core/Event/CWindowEvent.h"
	#include "Core/Event/CQuitEvent.h"
	#include "Core/Event/CEventCallback.h"

//--------------------------------------------------------
//Advise graphics drivers to use discrete GPU on notebooks
//--------------------------------------------------------

extern "C"
{
	// nVidia
	LAMBDA_API DWORD NvOptimusEnablement = 0x00000001;
	// AMD
	LAMBDA_API int AmdPowerXpressRequestHighPerformance = 1;
}


namespace Lambda
{
	//-------------------
	//CWindowsEnvironment
	//-------------------

	void CWindowsEnvironment::Init()
	{
		CEnvironment::Init();

		//Create window
		m_pWindow = DBG_NEW CWindowsWindow("Lambda Engine", 1280, 720);

		//Set eventcallback to the window
		IEventCallback* pEventCallback = DBG_NEW CObjectEventCallback<CWindowsEnvironment, CEvent>(this, &CWindowsEnvironment::OnEvent);
		m_pWindow->SetEventCallback(pEventCallback);
	}


	void CWindowsEnvironment::ProcessEvents()
	{
		MSG msg = {};
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//Send a quitevent
			if (msg.message == WM_QUIT)
			{
				CQuitEvent quitEvent = CQuitEvent(0);
				OnEvent(quitEvent);
			}
		}
	}


	void CWindowsEnvironment::Release()
	{
		SafeDelete(m_pWindow);
		delete this;
	}

	
	void CWindowsEnvironment::PrintF(const char* pFormat, ...)
	{
		constexpr uint32 MAX_BUFFER_COUNT = 1024;
		static char s_Buffer[MAX_BUFFER_COUNT];
	
		va_list args;
		va_start(args, pFormat);
		vsnprintf(s_Buffer, MAX_BUFFER_COUNT, pFormat, args);
		va_end(args);

		printf(s_Buffer);
		OutputDebugStringA(s_Buffer);
	}

	bool CWindowsEnvironment::OnEvent(const CEvent& event)
	{
		CEventDispatcher dispatcher;
		if (dispatcher.Dispatch(this, &CWindowsEnvironment::OnWindowMove, event))
			return true;
		else if (dispatcher.Dispatch(this, &CWindowsEnvironment::OnWindowClose, event))
			return true;
		else if (dispatcher.Dispatch(this, &CWindowsEnvironment::OnWindowResize, event))
			return true;
		else if (dispatcher.Dispatch(this, &CWindowsEnvironment::OnWindowFocusChanges, event))
			return true;
		else
			return CEnvironment::OnEvent(event);
	}
	
	
	bool CWindowsEnvironment::OnWindowMove(const CWindowMoveEvent& event)
	{
		LOG_ENVIRONMENT_INFO("Window Moved x: %d, y: %d\n", event.GetX(), event.GetY());
		return false;
	}


	bool CWindowsEnvironment::OnWindowFocusChanges(const CWindowFocusChangedEvent& event)
	{
		LOG_ENVIRONMENT_INFO("Window Focus: '%s'\n", event.HasFocus() ? "Has Focus" : "Does not have focus");
		return false;
	}


	bool CWindowsEnvironment::OnWindowResize(const CWindowResizeEvent& event)
	{
		LOG_ENVIRONMENT_INFO("Window Resized w: %d, h: %d\n", event.GetWidth(), event.GetHeight());
		return false;
	}
	
	
	bool CWindowsEnvironment::OnWindowClose(const CWindowClosedEvent&)
	{
		PostQuitMessage(0);
		return true;
	}
}
#endif