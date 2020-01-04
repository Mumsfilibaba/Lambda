#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Engine/Console.h"

	#include "Core/Utilities/StringUtilities.h"

	#include "Platform/Windows/WindowsWindow.h"
	#include "Platform/Windows/WindowsSystem.h"
	
	#define	NAME_APPWINDOW L"AppWindow"

namespace Lambda
{
	//--------------
	//CWindowsWindow
	//--------------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CWindowsWindow::CWindowsWindow(CWindowsSystem* pSystem)
		: IWindow(),
		m_pSystem(pSystem),
		m_hWindow(0),
		m_Height(0),
		m_Width(0),
		m_bHasFocus(false)
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
	bool CWindowsWindow::Initialize(const char* pTitle, uint32 width, uint32 height)
	{
		//Create window
		int32 error = 0;
		{
			WNDCLASSEX wc = {};
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc = CWindowsSystem::MessageProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = m_pSystem->GetHINSTANCE();
			wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
			wc.lpszMenuName = NULL;
			wc.lpszClassName = NAME_APPWINDOW;
			wc.hIconSm = 0;

			::RegisterClassEx(&wc);

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
			m_hWindow = CreateWindowEx(m_ExStyle, NAME_APPWINDOW, StringUtilities::ConvertFromString(pName).c_str(), m_Style, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, wc.hInstance, 0);
			if (m_hWindow == 0)
			{
				error = GetLastError();
				return false;
			}
			else
			{
				m_Width = width;
				m_Height = height;

				ShowWindow(m_hWindow, SW_NORMAL);
				return true;
			}
		}

		return false;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LRESULT CWindowsWindow::OnMessage(uint32 message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CREATE:
			CConsole::PrintLine("Window Created");
			break;

		case WM_DESTROY:
			CConsole::PrintLine("Window Destroyed");
			break;

		case WM_SIZE:
			m_Width		= LOWORD(lParam);
			m_Height	= HIWORD(lParam);
			break;

		case WM_MOVE:
			m_Position.x = (int32)(int16)LOWORD(lParam);
			m_Position.y = (int32)(int16)HIWORD(lParam);
			break;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
			m_bHasFocus = (message == WM_SETFOCUS);
			break;
		}

		return 0;
	}
}
#endif
