#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/CLogManager.h"
	#include "Utilities/StringHelper.h"
	#include "WindowsHelper.h"
	#include "CWindowClass.h"
	#include "CWindowsWindow.h"
	#include "CWindowsEnvironment.h"
	#include "Core/Event/CWindowEvent.h"
	#include "Core/Event/CKeyEvent.h"
	#include "Core/Event/CMouseEvent.h"

	#define	NAME_APPWINDOW L"AppWindow"

namespace Lambda
{
	//--------------
	//CWindowsWindow
	//--------------

	LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	CWindowsWindow::CWindowsWindow(const char* pTitle, uint32 width, uint32 height)
		: CWindowBase(),
		m_hWindow(0),
		m_Fullscreen(false),
		m_HasFocus(false),
		m_HasMouseCapture(false)
	{
		Init(pTitle, width, height);
		LOG_ENVIRONMENT_INFO("[LAMBDA ENGINE] Created window. w: %d, h: %d\n", width, height);
	}
	

	CWindowsWindow::~CWindowsWindow()
	{
		SetFullscreen(false);

		//Remove window
		if (IsWindow(m_hWindow))
		{
			DestroyWindow(m_hWindow);
			m_hWindow = 0;

			CWindowClass::Unregister(NAME_APPWINDOW);
		}
	}


	bool CWindowsWindow::HasFocus() const
	{
		return m_HasFocus;
	}


	uint32 CWindowsWindow::GetHeight() const
	{
		return m_Height;
	}


	uint32 CWindowsWindow::GetWidth() const
	{
		return m_Width;
	}


	bool CWindowsWindow::SetFullscreen(bool fullscreen)
	{
		if (m_Fullscreen == fullscreen)
		{
			return true;
		}


		if (fullscreen)
		{
			SetWindowRgn(m_hWindow, 0, FALSE);

			DEVMODE settings = {};
			memset(&settings, 0, sizeof(DEVMODE));
			settings.dmSize			= sizeof(DEVMODE);
			settings.dmPelsWidth	= GetSystemMetrics(SM_CXSCREEN);
			settings.dmPelsHeight	= GetSystemMetrics(SM_CYSCREEN);
			settings.dmBitsPerPel	= 32;
			settings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if (ChangeDisplaySettingsW(&settings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
			{
				m_Style	  = GetWindowLongW(m_hWindow, GWL_STYLE);
				m_ExStyle = GetWindowLongW(m_hWindow, GWL_EXSTYLE);

				SetWindowLongW(m_hWindow, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
				SetWindowLongW(m_hWindow, GWL_EXSTYLE, WS_EX_APPWINDOW);
				SetWindowPos(m_hWindow, HWND_TOP, 0, 0, m_Width, m_Height, SWP_FRAMECHANGED);
				ShowWindow(m_hWindow, SW_SHOWMAXIMIZED);

				InvalidateRect(m_hWindow, 0, TRUE);
                
                m_Fullscreen = true;

				LOG_ENVIRONMENT_WARNING("[LAMBDA ENGINE] Switched into fullscreen\n");
			}
			else 
			{
				LOG_ENVIRONMENT_ERROR("[LAMBDA ENGINE] Failed to switch into fullscreen\n");
				return false;
			}
		}
		else
		{
			if (ChangeDisplaySettingsW(NULL, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
			{
				SetWindowLongW(m_hWindow, GWL_STYLE, m_Style);
				SetWindowLongW(m_hWindow, GWL_EXSTYLE, m_ExStyle);
				SetWindowPos(m_hWindow, HWND_TOP, 0, 0, m_Width, m_Height, SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_FRAMECHANGED);
				ShowWindow(m_hWindow, SW_SHOW);

                m_Fullscreen = false;

				LOG_ENVIRONMENT_INFO("[LAMBDA ENGINE] Switched into windowed mode\n");
			}
            else
            {
				LOG_ENVIRONMENT_ERROR("[LAMBDA ENGINE] Failed to switch from fullscreen\n");
                return false;
            }
		}

		return true;
	}


	bool CWindowsWindow::HasMouseCapture() const
	{
		return m_HasMouseCapture;
	}


	bool CWindowsWindow::GetFullscreen() const
	{
		return m_Fullscreen;
	}


	void CWindowsWindow::Init(const char* pTitle, uint32 width, uint32 height)
	{
		//Should only be called once
		LAMBDA_ASSERT(m_hWindow == 0);

		int32 error = 0;
		{
			WNDCLASSEX wc = {};
			wc.cbSize		 = sizeof(WNDCLASSEX);
			wc.style		 = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc	 = WindowEventCallback;
			wc.cbClsExtra	 = 0;
			wc.cbWndExtra	 = 0;
			wc.hInstance	 = static_cast<HINSTANCE>(GetModuleHandle(NULL));
			wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
			wc.lpszMenuName	 = NULL;
			wc.lpszClassName = NAME_APPWINDOW;
			wc.hIconSm		 = 0;

			CWindowClass::Register(wc);

			//Set client area size
			m_Style		= WS_OVERLAPPEDWINDOW;
			m_ExStyle	= 0;
			RECT rect	= { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
			AdjustWindowRect(&rect, m_Style, false);

            
            //Make sure name is not nullptr
            const char* pName = pTitle;
            if (pName == nullptr)
                pName = "";
            
			//Create window
			SetLastError(0);
			m_hWindow = CreateWindowEx(m_ExStyle, NAME_APPWINDOW, StringToWidestring(pName).c_str(), m_Style, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, wc.hInstance, 0);
			if (m_hWindow == 0)
			{
				error = GetLastError();
			}
			else
			{
				m_Width	 = width;
				m_Height = height;

				//Set userdata so we can retrive this-pointer when handling events
				SetWindowLongPtr(m_hWindow, GWLP_USERDATA, reinterpret_cast<uintptr_t>(this));
				ShowWindow(m_hWindow, SW_NORMAL);
			}
		}

		//Set fullscreen
		SetFullscreen(false);
	}


	LRESULT CWindowsWindow::OnEvent(uint32 msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
		{
			CWindowClosedEvent event = CWindowClosedEvent();
			DispatchEvent(event);
			break;
		}
		
		case WM_SIZE:
		{
			CWindowResizeEvent event = CWindowResizeEvent(uint32(LOWORD(lParam)), uint32(HIWORD(lParam)));
			DispatchEvent(event);
			break;
		}

		case WM_MOVE:
		{
			CWindowMoveEvent event = CWindowMoveEvent(uint32(LOWORD(lParam)), uint32(HIWORD(lParam)));
			DispatchEvent(event);
			break;
		}
		
		case WM_KEYDOWN:
		{
			CWindowsEnvironment& environment = GET_WINDOWS_ENVIRONMENT();
			EKey key = environment.ConvertKeyFromWindows(uint32(wParam));

			CKeyPressedEvent event = CKeyPressedEvent(key, GetKeyModifers(), uint32(LOWORD(lParam)));
			DispatchEvent(event);
			break;
		}
		case WM_KEYUP:
		{
			CWindowsEnvironment& environment = GET_WINDOWS_ENVIRONMENT();
			EKey key = environment.ConvertKeyFromWindows(uint32(wParam));

			CKeyReleasedEvent event = CKeyReleasedEvent(key, GetKeyModifers());
			DispatchEvent(event);
			break;
		}

		case WM_CHAR:
		{
			CKeyTypedEvent event = CKeyTypedEvent(uint32(wParam));
			DispatchEvent(event);
			break;
		}

		case WM_MOUSEMOVE:
		{
			CMouseMovedEvent event = CMouseMovedEvent(uint32(GET_X_LPARAM(lParam)), uint32(GET_Y_LPARAM(lParam)));
			DispatchEvent(event);
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
			EMouseButton button = MOUSEBUTTON_UNKNOWN;
			if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP)
				button = MOUSEBUTTON_LEFT;
			else if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONUP)
				button = MOUSEBUTTON_MIDDLE;
			else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONUP)
				button = MOUSEBUTTON_RIGHT;
			else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
				button = MOUSEBUTTON_BACKWARD;
			else
				button = MOUSEBUTTON_FORWARD;

			uint32 modifiers = GetKeyModifers();
			if (msg == WM_LBUTTONUP || msg == WM_MBUTTONUP || msg == WM_RBUTTONUP || msg == WM_XBUTTONUP)
			{
				//When the mousebutton is released we also release the mousecapture
				ReleaseCapture();
				m_HasMouseCapture = false;

				//Send event
				CMouseButtonReleasedEvent event = CMouseButtonReleasedEvent(button, modifiers);
				DispatchEvent(event);
			}
			else
			{
				//Mouse is down so we capture the mouse
				SetCapture(m_hWindow);
				m_HasMouseCapture = true;

				//Set event
				CMouseButtonPressedEvent event = CMouseButtonPressedEvent(button, modifiers);
				DispatchEvent(event);
			}
			break;
		}
		
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		{
			float value				= float(GET_WHEEL_DELTA_WPARAM(wParam)) / float(WHEEL_DELTA);
			float horizontalValue	= 0.0f;
			float verticalValue		= 0.0f;
			if (msg == WM_MOUSEHWHEEL)
				horizontalValue = value;
			else
				verticalValue = value;

			CMouseScrolledEvent event = CMouseScrolledEvent(horizontalValue, verticalValue);
			DispatchEvent(event);
			break;
		}

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
			//Update has focus
			m_HasFocus = msg == WM_SETFOCUS;

			CWindowFocusChangedEvent event = CWindowFocusChangedEvent(m_HasFocus);
			DispatchEvent(event);
			break;
		}

		default:
			return DefWindowProc(m_hWindow, msg, wParam, lParam);
		}

		return LRESULT(0);
	}


	LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//Retrive userdata to get the pointer to the windows
		CWindowsWindow* pWindow = reinterpret_cast<CWindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (pWindow)
			return pWindow->OnEvent(msg, wParam, lParam);
		else
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}
#endif
