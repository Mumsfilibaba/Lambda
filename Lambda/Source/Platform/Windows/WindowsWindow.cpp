#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Engine/Console.h"

	#include "Core/Utilities/StringUtilities.h"

	#include "Platform/Windows/WindowsWindow.h"
    #include "Platform/Windows/WindowsKeyboard.h"
	#include "Platform/Windows/WindowsApplication.h"

namespace Lambda
{
	//--------------
	//CWindowsWindow
	//--------------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CWindowsWindow::CWindowsWindow(CWindowsApplication* pApplication)
		: CWindow(),
        m_pApplication(pApplication),
		m_hWindow(0),
		m_Height(0),
		m_Width(0),
		m_ExStyle(0),
		m_Style(0),
		m_Position(),
		m_bHasFocus(false),
		m_bIsFullscreen(false),
		m_bIsMiniaturized(false)
	{
		//Init position
		m_Position = { 0, 0 };
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CWindowsWindow::~CWindowsWindow()
	{
		if (::IsWindow(m_hWindow))
		{
			::DestroyWindow(m_hWindow);
			m_hWindow = 0;
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CWindowsWindow::Init(const SWindowProps& props)
	{
		//Create window
		int32 error = 0;
		{
			//Set client area size
			m_Style   = WS_OVERLAPPEDWINDOW;
			m_ExStyle = 0;
			RECT rect = { 0, 0, LONG(props.Width), LONG(props.Height) };

			::AdjustWindowRect(&rect, m_Style, false);

			//Create window
			const char* pTitle = (props.pTitle != nullptr) ? props.pTitle : "";
			std::wstring convertedTitle = StringUtilities::ConvertFromString(pTitle);
			uint32 width  = rect.right - rect.left;
			uint32 height = rect.bottom - rect.top;
			HINSTANCE hInstance = CWindowsApplication::GetApplicationInstance();

			::SetLastError(0);
			m_hWindow = ::CreateWindowEx(m_ExStyle, CWindowsWindow::WindowClass(), convertedTitle.c_str(), m_Style, 0, 0, width, height, 0, 0, hInstance, 0);
			if (m_hWindow == 0)
			{
				error = ::GetLastError();
				return false;
			}
			else
			{
				m_Width  = width;
				m_Height = height;

				//Set userdata so we can retrive this-pointer when handling events
				::SetWindowLongPtr(m_hWindow, GWLP_USERDATA, reinterpret_cast<uintptr_t>(this));
				return true;
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LRESULT CWindowsWindow::ProcessMessage(uint32 message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_SIZE:
			{
				//Set members
				if (wParam == SIZE_MAXIMIZED)
					m_bIsMiniaturized = false;
				else if (wParam == SIZE_MINIMIZED)
					m_bIsMiniaturized = true;

				m_Width  = LOWORD(lParam);
				m_Height = HIWORD(lParam);

				break;
			}

			case WM_MOVE:
			{
				//Set members
				m_Position.x = (int32)(int16)LOWORD(lParam);
				m_Position.y = (int32)(int16)HIWORD(lParam);
				break;
			}

			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			{
				//Set members
				m_bHasFocus = (message == WM_SETFOCUS);
				break;
			}
		}

		return 0;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::SetDisplayMode(EDisplayMode displayMode)
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::Show()
	{
		::ShowWindow(m_hWindow, SW_NORMAL);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::Hide()
	{
		::ShowWindow(m_hWindow, SW_HIDE);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::Maximize()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::Minimize()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::SetFocus()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::SetSize(uint32 width, uint32 height)
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsWindow::SetPosition(uint32 x, uint32 y)
	{
	}
}
#endif
