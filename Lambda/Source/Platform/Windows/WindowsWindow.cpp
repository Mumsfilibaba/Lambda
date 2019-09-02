#include "LambdaPch.h"
#include "System/Log.h"
#include "Utilities/StringHelper.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "WindowsWindow.h"
	#include "WindowClass.h"
	#include "WindowsInput.h"
	#define	NAME_APPWINDOW L"AppWindow"

namespace Lambda
{
	LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


	IWindow* IWindow::Create(const WindowDesc& desc)
	{
		return DBG_NEW WindowsWindow(desc);
	}


	WindowsWindow::WindowsWindow(const WindowDesc& desc)
		: m_OnEvent(nullptr),
		m_Wnd(0),
		m_EventBackLog()
	{
		Init(desc);

		LOG_SYSTEM_INFO("Creating window. w: %d, h: %d\n", desc.Width, desc.Height);
	}
	

	WindowsWindow::~WindowsWindow()
	{
		if (IsWindow(m_Wnd))
		{
			DestroyWindow(m_Wnd);
			m_Wnd = 0;

			WindowClass::Unregister(NAME_APPWINDOW);
		}
	}
	

	void WindowsWindow::SetEventCallback(EventCallback callback)
	{
		m_OnEvent = callback;
	}


	void WindowsWindow::OnUpdate() const
	{
		MSG msg = {};
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    
    IGraphicsDevice* WindowsWindow::GetGraphicsDevice() const
    {
        return nullptr;
    }
    

	void* WindowsWindow::GetNativeHandle() const
	{
		return (void*)m_Wnd;
	}


	uint32 WindowsWindow::GetHeight() const
	{
		return m_Height;
	}


	uint32 WindowsWindow::GetWidth() const
	{
		return m_Width;
	}


	void WindowsWindow::Init(const WindowDesc& desc)
	{
		//Should only be called once
		assert(m_Wnd == 0);

		int32 error = 0;
		{
			WNDCLASSEX wc = {};
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
			wc.lpfnWndProc = WindowEventCallback;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = static_cast<HINSTANCE>(GetModuleHandle(NULL));
			wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wc.hCursor = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
			wc.lpszMenuName = NULL;
			wc.lpszClassName = NAME_APPWINDOW;
			wc.hIconSm = 0;

			WindowClass::Register(wc);

			//Set client area size
			int32 style = WS_OVERLAPPEDWINDOW;
			RECT rect = { 0, 0, static_cast<LONG>(desc.Width), static_cast<LONG>(desc.Height) };
			AdjustWindowRect(&rect, style, false);

			//Create window
			SetLastError(0);
			m_Wnd = CreateWindowEx(0, NAME_APPWINDOW, StringToWidestring(desc.pTitle).c_str(), style, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0, 0, wc.hInstance, 0);
			if (m_Wnd == 0)
			{
				error = GetLastError();
			}
			else
			{
				m_Width = desc.Width;
				m_Height = desc.Height;

				//Set userdata so we can retrive this-pointer when handling events
				SetWindowLongPtr(m_Wnd, GWLP_USERDATA, reinterpret_cast<uintptr_t>(this));
				ShowWindow(m_Wnd, SW_NORMAL);
			}
		}
	}


	LRESULT WindowsWindow::OnEvent(uint32 msg, WPARAM wParam, LPARAM lParam)
	{
		Event event = {};
		event.Type = EVENT_TYPE_UNKNOWN;
		event.MouseScrollEvent.Vertical = false;

		switch (msg)
		{
		case WM_DESTROY:
		{
			event.Type = EVENT_TYPE_WINDOW_CLOSED;
			break;
		}
		
		case WM_SIZE:
		{
			event.Type = EVENT_TYPE_WINDOW_RESIZE;
			event.WindowResize.Width = LOWORD(lParam);
			event.WindowResize.Height = HIWORD(lParam);
			break;
		}

		case WM_MOVE:
		{
			event.Type = EVENT_TYPE_WINDOW_MOVE;
			event.WindowMove.PosX = LOWORD(lParam);
			event.WindowMove.PosY = HIWORD(lParam);
			break;
		}
		
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			event.Type = msg == WM_KEYDOWN ? EVENT_TYPE_KEYDOWN : EVENT_TYPE_KEYUP;
			event.KeyEvent.KeyCode = WindowsInput::ConvertWindowsKey(uint32(wParam));
			event.KeyEvent.RepeatCount = LOWORD(lParam);
			break;
		}

		case WM_CHAR:
		{
			event.Type = EVENT_TYPE_TEXT;
			event.TextEvent.Character = wchar_t(wParam);
			break;
		}

		case WM_MOUSEMOVE:
		{
			event.Type = EVENT_TYPE_MOUSE_MOVED;
			event.MouseMoveEvent.PosX = GET_X_LPARAM(lParam);
			event.MouseMoveEvent.PosY = GET_Y_LPARAM(lParam);
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
		{
			event.Type = EVENT_TYPE_MOUSE_BUTTONUP;
			event.MouseButtonEvent.Button = WindowsInput::ConvertWindowsButton(uint32(wParam));
			break;
		}

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
		{
			event.Type = EVENT_TYPE_MOUSE_BUTTONDOWN;
			event.MouseButtonEvent.Button = WindowsInput::ConvertWindowsButton(uint32(wParam));
			break;
		}
		
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		{
			event.Type = EVENT_TYPE_MOUSE_SCROLLED;
            event.MouseScrollEvent.Vertical = (msg == WM_MOUSEHWHEEL) ? false : true;
			event.MouseScrollEvent.Value = float(GET_WHEEL_DELTA_WPARAM(wParam)) / float(WHEEL_DELTA);
			break;
		}

		case WM_SETFOCUS:
		{
			event.Type = EVENT_TYPE_FOCUS_CHANGED;
			event.FocusChanged.HasFocus = true;
			break;
		}

		case WM_KILLFOCUS:
		{
			event.Type = EVENT_TYPE_FOCUS_CHANGED;
			event.FocusChanged.HasFocus = false;
			break;
		}

		default:
			return DefWindowProc(m_Wnd, msg, wParam, lParam);
		}

		if (m_OnEvent)
		{
			//When a eventhandler is registered, then we handled all the backloged items
			if (m_EventBackLog.size() > 0)
			{
				for (auto& e : m_EventBackLog)
					m_OnEvent(e);

				m_EventBackLog.clear();
			}

			m_OnEvent(event);
		}
		else
		{
			//If a eventcallback is not registered then we put the event in the backlog
			m_EventBackLog.push_back(event);
		}

		return LRESULT(0);
	}


	LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//Retrive userdata to get the pointer to the windows
		WindowsWindow* pWindow = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (pWindow)
			return pWindow->OnEvent(msg, wParam, lParam);
		else
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}
#endif
