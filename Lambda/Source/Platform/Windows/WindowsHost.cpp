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
				this->OnQuit(int32(msg.wParam));
		}
	}


	void WindowsHost::Release()
	{
		SafeDelete(m_pWindow);
		delete this;
	}

	
	void WindowsHost::PrintF(const char* pFormat, ...)
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
	
	
	void WindowsHost::OnWindowMove(uint32 x, uint32 y)
	{
		LOG_HOST_INFO("Window Moved x: %d, y: %d\n", x, y);
	}


	void WindowsHost::OnWindowFocusChanges(bool hasFocus)
	{
		LOG_HOST_INFO("Window Focus %s\n", hasFocus ? "Has Focus" : "Does not have focus");
	}


	void WindowsHost::OnWindowResize(uint32 width, uint32 height)
	{
		LOG_HOST_INFO("Window Resized w: %d, h: %d\n", width, height);
	}
	
	
	void WindowsHost::OnWindowClose()
	{
		PostQuitMessage(0);
	}
}