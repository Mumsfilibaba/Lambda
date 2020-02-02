#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Platform/Windows/WindowsPlatform.h"
	#include "Platform/Windows/WindowsWindow.h"
	#include "Platform/Windows/WindowsConsoleOutput.h"
	#include "Platform/Windows/WindowsTime.h"

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

		CWindowsTime::Init();

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
