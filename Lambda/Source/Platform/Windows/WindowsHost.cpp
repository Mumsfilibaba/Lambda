#include "LambdaPch.h"
#include "WindowsHost.h"
#include "WindowsWindow.h"
#include "Core/WindowEventDispatcher.h"

namespace Lambda
{
	//-----------
	//WindowsHost
	//-----------

	void WindowsHost::Init()
	{
		//Add windowlistener
		WindowEventDispatcher::Get().AddListener(this);

		//Create window
		m_pWindow = DBG_NEW WindowsWindow("Lambda Engine", 1280, 720);
	}


	void WindowsHost::ProcessEvents()
	{
		MSG msg = {};
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				this->OnQuit();
		}
	}


	void WindowsHost::Release()
	{
		SafeDelete(m_pWindow);
		delete this;
	}
	
	
	void WindowsHost::OnWindowResize(uint32 width, uint32 height)
	{
	}
	
	
	void WindowsHost::OnWindowDestroy()
	{
		PostQuitMessage(0);
	}
}