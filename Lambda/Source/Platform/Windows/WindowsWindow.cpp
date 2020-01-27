#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Core/Utilities/StringUtilities.h"

	#include "Platform/Windows/WindowsWindow.h"
	#include "Platform/Windows/WindowsPlatform.h"

namespace Lambda
{
	CWindowsWindow::CWindowsWindow(const SWindowDesc& desc)
		: CGenericWindow(),
		m_hWindow(0),
		m_Height(0),
		m_Width(0),
		m_Style(0),
		m_ExStyle(0)
	{
		Init(desc);
	}
	
	CWindowsWindow::~CWindowsWindow()
	{
		if (::IsWindow(m_hWindow))
		{
			::DestroyWindow(m_hWindow);
			m_hWindow = 0;
		}
	}

	void CWindowsWindow::Show()
	{
		if (::IsWindow(m_hWindow))
			::ShowWindow(m_hWindow, SW_NORMAL);
	}
		
	void CWindowsWindow::Init(const SWindowDesc& desc)
	{
		int32 error = 0;

		//Set client area size
		m_Style		= WS_OVERLAPPEDWINDOW;
		m_ExStyle	= 0;
		RECT rect = { 0, 0, LONG(desc.Width), LONG(desc.Height) };

		::AdjustWindowRect(&rect, m_Style, false);

		//Create window
		const char* pTitle = (desc.pTitle != nullptr) ? desc.pTitle : "";
		std::wstring convertedTitle = StringUtilities::ConvertFromString(pTitle);
		uint32 width = rect.right - rect.left;
		uint32 height = rect.bottom - rect.top;
		HINSTANCE hInstance = CWindowsPlatform::GetApplicationInstance();

		::SetLastError(0);
		m_hWindow = ::CreateWindowEx(m_ExStyle, CWindowsWindow::GetWindowClass(), convertedTitle.c_str(), m_Style, 0, 0, width, height, 0, 0, hInstance, 0);
		if (m_hWindow == 0)
		{
			error = ::GetLastError();
		}
		else
		{
			m_Width = width;
			m_Height = height;

			//Set userdata so we can retrive this-pointer when handling events
			::SetWindowLongPtr(m_hWindow, GWLP_USERDATA, reinterpret_cast<uintptr_t>(this));
		}
	}

	LRESULT CWindowsWindow::ProcessMessage(uint32 message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_SIZE:
			{
				//Set members
				/*if (wParam == SIZE_MAXIMIZED)
					m_bIsMiniaturized = false;
				else if (wParam == SIZE_MINIMIZED)
					m_bIsMiniaturized = true;*/

				m_Width = LOWORD(lParam);
				m_Height = HIWORD(lParam);

				return 0;
			}

			case WM_MOVE:
			{
				//Set members
				//m_Position.x = (int32)(int16)LOWORD(lParam);
				//m_Position.y = (int32)(int16)HIWORD(lParam);
				return 0;
			}

			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			{
				//Set members
				//m_bHasFocus = (message == WM_SETFOCUS);
				return 0;
			}
		}

		return DefWindowProc(m_hWindow, message, wParam, lParam);;
	}

	void CWindowsWindow::RegisterWindowClass(HINSTANCE hInstance)
	{
		//Register window class
		WNDCLASSEX wc = {};
		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc		= CWindowsPlatform::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= CWindowsWindow::GetWindowClass();
		wc.hIconSm			= 0;

		::RegisterClassEx(&wc);
	}
	
	void CWindowsWindow::UnregisterWindowClass(HINSTANCE hInstance)
	{
		::UnregisterClass(CWindowsWindow::GetWindowClass(), hInstance);
	}
}
#endif