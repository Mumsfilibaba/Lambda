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
		: IWindow(),
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
	void CWindowsWindow::SetFullscreen(bool bFullscreen)
	{
		if (m_bIsFullscreen != bFullscreen)
		{

		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CWindowsWindow::Init(const char* pTitle, uint32 width, uint32 height)
	{
		//Create window
		int32 error = 0;
		{
			//Set client area size
			m_Style = WS_OVERLAPPEDWINDOW;
			m_ExStyle = 0;
			RECT rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
			AdjustWindowRect(&rect, m_Style, false);

			//Make sure name is not nullptr
			const char* pName = pTitle;
			if (pName == nullptr)
			{
				pName = "";
			}

			//Create window
			SetLastError(0);
			m_hWindow = CreateWindowEx(m_ExStyle, CWindowsWindow::WindowClass(), StringUtilities::ConvertFromString(pName).c_str(), m_Style, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, m_pApplication->GetHINSTANCE(), 0);
			if (m_hWindow == 0)
			{
				error = GetLastError();
				return false;
			}
			else
			{
				m_Width  = width;
				m_Height = height;

                //Set userdata so we can retrive this-pointer when handling events
                SetWindowLongPtr(m_hWindow, GWLP_USERDATA, reinterpret_cast<uintptr_t>(this));
				ShowWindow(m_hWindow, SW_NORMAL);
				return true;
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LRESULT CWindowsWindow::HandleMessage(uint32 message, WPARAM wParam, LPARAM lParam)
	{
        switch (message)
        {
        case WM_CREATE:
        {
            CConsole::PrintLine("Window Created");
            break;
        }

        case WM_DESTROY:
        {
            CConsole::PrintLine("Window Destroyed");
            break;
        }

        case WM_SIZE:
        {
            //Set members
            m_Width = LOWORD(lParam);
            m_Height = HIWORD(lParam);
            return 0;
        }

        case WM_MOVE:
        {
            //Set members
            m_Position.x = (int32)(int16)LOWORD(lParam);
            m_Position.y = (int32)(int16)HIWORD(lParam);
            return 0;
        }

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        {
            //Set members
            m_bHasFocus = (message == WM_SETFOCUS);
            return 0;
        }
        }
	}
}
#endif
