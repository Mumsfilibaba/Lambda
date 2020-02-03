#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsPlatform.h"
	#include "Platform/Windows/WindowsTime.h"
	#include "Platform/Windows/WindowsInput.h"
	#include "Platform/Windows/WindowsWindow.h""
	#include "Platform/Windows/WindowsConsoleOutput.h"

namespace Lambda
{
	HINSTANCE CWindowsPlatform::s_hInstance = 0;

	IConsoleOutput* CWindowsPlatform::CreateConsoleOutput()
	{
		return DBG_NEW CWindowsConsoleOutput();
	}

	IWindow* CWindowsPlatform::CreateWindow(const SWindowDesc& desc)
	{
		return DBG_NEW CWindowsWindow(desc);
	}

	void CWindowsPlatform::Init(HINSTANCE hInstance)
	{
		//Activate debug memory check
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		//Set global instance
		s_hInstance = hInstance;

		//Init time (Get frequency)
		CWindowsTime::Init();
		
		//Init input (KeyTable etc.)
		CWindowsInput::Init();

		//Register Window class for creation of main window
		CWindowsWindow::RegisterWindowClass(hInstance);
	}

	void CWindowsPlatform::Release()
	{
		CWindowsWindow::UnregisterWindowClass(s_hInstance);
	}

	void CWindowsPlatform::PollEvents()
	{
		MSG message = {};
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
	
	void CWindowsPlatform::MessageBox(const char* pCaption, const char* pText, uint32 type)
	{
		uint32 uType = 0;
		if (type & EMessageBox::MESSAGE_BOX_OK)
			uType |= MB_OK;
		if (type & EMessageBox::MESSAGE_BOX_ERRORICON)
			uType |= MB_ICONERROR;

		MessageBoxA(0, pText, pCaption, uType);
	}

	void CWindowsPlatform::DebuggerOutput(const char* pMessage)
	{
		OutputDebugStringA(pMessage);
	}

	LRESULT CWindowsPlatform::WndProc(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam)
	{
		//Dispatch input messages to input class
		switch (message)
		{
			case WM_KEYUP:
			case WM_KEYDOWN:
			{
				bool bKeyDown = (message == WM_KEYDOWN);
				CWindowsInput::OnKeyboardMessage(uint32(wParam), bKeyDown);
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
				CWindowsInput::OnMouseButtonMessage(message, wParam);
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
