#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsPlatform.h"
	#include "Platform/Windows/WindowsTime.h"
	#include "Platform/Windows/WindowsInput.h"
	#include "Platform/Windows/WindowsWindow.h"
	#include "Platform/Windows/WindowsConsoleOutput.h"

namespace Lambda
{
	HINSTANCE WindowsPlatform::s_hInstance = 0;

	IConsoleOutput* WindowsPlatform::CreateConsoleOutput()
	{
		return DBG_NEW CWindowsConsoleOutput();
	}

	IWindow* WindowsPlatform::CreateWindow(const SWindowDesc& desc)
	{
		return DBG_NEW CWindowsWindow(desc);
	}

	void WindowsPlatform::Init(HINSTANCE hInstance)
	{
		//Activate debug memory check
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		//Set global instance
		s_hInstance = hInstance;

		//Init time (Get frequency)
		WindowsTime::Init();
		
		//Init input (KeyTable etc.)
		WindowsInput::Init();

		//Register Window class for creation of main window
		CWindowsWindow::RegisterWindowClass(hInstance);
	}

	void WindowsPlatform::Release()
	{
		CWindowsWindow::UnregisterWindowClass(s_hInstance);
	}

	void WindowsPlatform::PollEvents()
	{
		MSG message = {};
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	
	void WindowsPlatform::MessageBox(const char* pCaption, const char* pText, uint32 type)
	{
		uint32 uType = 0;
		if (type & EMessageBox::MESSAGE_BOX_OK)
			uType |= MB_OK;
		if (type & EMessageBox::MESSAGE_BOX_ERRORICON)
			uType |= MB_ICONERROR;

		MessageBoxA(0, pText, pCaption, uType);
	}

	void WindowsPlatform::DebuggerOutput(const char* pMessage)
	{
		OutputDebugStringA(pMessage);
	}

	LRESULT WindowsPlatform::WndProc(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam)
	{
		//Dispatch input messages to input class
		switch (message)
		{
			case WM_KEYUP:
			case WM_KEYDOWN:
			{
				bool bKeyDown = (message == WM_KEYDOWN);
				WindowsInput::OnKeyboardMessage(uint32(wParam), bKeyDown);
				break;
			}

			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
			case WM_XBUTTONUP:
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_XBUTTONDOWN:
			{
				WindowsInput::OnMouseButtonMessage(message, wParam);
				break;
			}
		}

		//Dispatch events to window
		CWindowsWindow* pWindow = (CWindowsWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (pWindow)
		{
			LRESULT result = pWindow->ProcessMessage(message, wParam, lParam);
			return result;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
#endif
