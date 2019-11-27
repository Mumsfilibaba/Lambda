#include "LambdaPch.h"
#include "WindowsEnvironment.h"
#include "WindowsWindow.h"
#include "Core/WindowEventDispatcher.h"

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
	//-----------
	//WindowsHost
	//-----------

	void WindowsEnvironment::Init()
	{
		//Add windowlistener
		WindowEventDispatcher::Get().AddListener(this);

		//Create window
		m_pWindow = DBG_NEW WindowsWindow("Lambda Engine", 1280, 720);
	}


	void WindowsEnvironment::ProcessEvents()
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


	void WindowsEnvironment::Release()
	{
		SafeDelete(m_pWindow);
		delete this;
	}

	
	void WindowsEnvironment::PrintF(const char* pFormat, ...)
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
	
	
	void WindowsEnvironment::OnWindowMove(uint32 x, uint32 y)
	{
		LOG_HOST_INFO("Window Moved x: %d, y: %d\n", x, y);
	}


	void WindowsEnvironment::OnWindowFocusChanges(bool hasFocus)
	{
		LOG_HOST_INFO("Window Focus %s\n", hasFocus ? "Has Focus" : "Does not have focus");
	}


	void WindowsEnvironment::OnWindowResize(uint32 width, uint32 height)
	{
		LOG_HOST_INFO("Window Resized w: %d, h: %d\n", width, height);
	}
	
	
	void WindowsEnvironment::OnWindowClose()
	{
		PostQuitMessage(0);
	}
}