#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Core/Event/Event.h"
	#include "Core/Event/EventHandler.h"

	#include "Core/Utilities/StringUtilities.h"

	#include "Platform/Windows/WindowsWindow.h"
	#include "Platform/Windows/WindowsPlatform.h"
	#include "Platform/Windows/WindowsKeyboard.h"

namespace Lambda
{
	CWindowsWindow::CWindowsWindow(const SWindowDesc& desc)
		: CCommonWindow(),
		m_hWindow(0),
		m_Height(0),
		m_Width(0),
		m_Style(0),
		m_ExStyle(0),
		m_bIsMiniaturized(false)
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

	void CWindowsWindow::AddEventListener(IEventHandler* pEventListener)
	{
		if (pEventListener)
			m_EventListeners.emplace_back(pEventListener);
	}

	void CWindowsWindow::RemoveEventListener(IEventHandler* pEventListener)
	{
		if (pEventListener)
		{
			for (auto it = m_EventListeners.begin(); it != m_EventListeners.end(); it++)
			{
				if (*it == pEventListener)
				{
					m_EventListeners.erase(it);
					break;
				}
			}
		}
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

	void CWindowsWindow::DispatchEvent(const SEvent& event)
	{
		for (IEventHandler* pEventListener : m_EventListeners)
			pEventListener->OnEvent(event);
	}

	LRESULT CWindowsWindow::ProcessMessage(uint32 message, WPARAM wParam, LPARAM lParam)
	{
		SEvent event = {};
		switch (message)
		{
			case WM_SIZE:
			{
				event.EventType = EEventType::EVENT_TYPE_WINDOW_RESIZE;
				if (wParam == SIZE_MAXIMIZED)
				{
					event.WindowResizeEvent.ResizeType = EResizeType::RESIZE_TYPE_MAXIMIZE;
					m_bIsMiniaturized = false;
				}
				else if (wParam == SIZE_MINIMIZED)
				{
					event.WindowResizeEvent.ResizeType = EResizeType::RESIZE_TYPE_MINIMIZE;
					m_bIsMiniaturized = true;
				}
				else
				{
					event.WindowResizeEvent.ResizeType = EResizeType::RESIZE_TYPE_STANDARD;
				}

				event.WindowResizeEvent.Width	= m_Width	= LOWORD(lParam);
				event.WindowResizeEvent.Height	= m_Height	= HIWORD(lParam);
				DispatchEvent(event);

				return 0;
			}

			case WM_MOVE:
			{
				event.EventType = EEventType::EVENT_TYPE_WINDOW_MOVE;
				event.WindowMoveEvent.x = (int32)(int16)LOWORD(lParam);
				event.WindowMoveEvent.y = (int32)(int16)HIWORD(lParam);
				DispatchEvent(event);

				return 0;
			}

			case WM_SETFOCUS:
			case WM_KILLFOCUS:
			{
				event.EventType = EEventType::EVENT_TYPE_WINDOW_FOCUS;
				event.WindowFocusEvent.bHasFocus = (message == WM_SETFOCUS);
				DispatchEvent(event);

				return 0;
			}

			case WM_KEYUP:
			case WM_KEYDOWN:
			{
				event.EventType = (message == WM_KEYUP) ? EEventType::EVENT_TYPE_KEY_RELEASED : EEventType::EVENT_TYPE_KEY_PRESSED;
				event.KeyEvent.Key			= CWindowsKeyboard::ConvertVirtualKey(uint32(wParam));
				event.KeyEvent.Modifiers	= CWindowsKeyboard::GetModifierKeys();
				event.KeyEvent.RepeatCount	= uint32(LOWORD(lParam));

				DispatchEvent(event);
				return 0;
			}

			case WM_CHAR:
			{
				event.EventType = EEventType::EVENT_TYPE_KEY_TYPED;
				event.KeyTypedEvent.Character = uint32(wParam);
				DispatchEvent(event);

				return 0;
			}

			case WM_MOUSEMOVE:
			{
				event.EventType = EEventType::EVENT_TYPE_MOUSE_MOVED;
				event.MouseMoveEvent.x = uint32(GET_X_LPARAM(lParam));
				event.MouseMoveEvent.y = uint32(GET_Y_LPARAM(lParam));
				DispatchEvent(event);

				return 0;
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
				EMouseButton button = EMouseButton::MOUSEBUTTON_UNKNOWN;
				if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
					button = EMouseButton::MOUSEBUTTON_LEFT;
				else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
					button = EMouseButton::MOUSEBUTTON_MIDDLE;
				else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
					button = EMouseButton::MOUSEBUTTON_RIGHT;
				else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
					button = EMouseButton::MOUSEBUTTON_BACKWARD;
				else
					button = EMouseButton::MOUSEBUTTON_FORWARD;

				event.MouseButtonEvent.Button = button;
				event.MouseButtonEvent.Modifiers = CWindowsKeyboard::GetModifierKeys();
				if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP || message == WM_XBUTTONUP)
				{
					//When the mousebutton is released we also release the mousecapture
					::ReleaseCapture();
					event.EventType = EEventType::EVENT_TYPE_MOUSE_BUTTON_RELEASED;
				}
				else
				{
					//Mouse is down so we capture the mouse
					::SetCapture(m_hWindow);
					event.EventType = EEventType::EVENT_TYPE_MOUSE_BUTTON_PRESSED;
				}
				DispatchEvent(event);

				return 0;
			}

			case WM_MOUSEWHEEL:
			case WM_MOUSEHWHEEL:
			{
				event.EventType = EEventType::EVENT_TYPE_MOUSE_SCROLLED;
				float value = float(GET_WHEEL_DELTA_WPARAM(wParam)) / float(WHEEL_DELTA);
				float horizontalValue = 0.0f;
				float verticalValue = 0.0f;
				if (message == WM_MOUSEHWHEEL)
					horizontalValue = value;
				else
					verticalValue = value;

				event.MouseScrolledEvent.Horizontal = horizontalValue;
				event.MouseScrolledEvent.Vertical	= verticalValue;
				DispatchEvent(event);

				return 0;
			}

			case WM_DESTROY:
			{
				event.EventType = EEventType::EVENT_TYPE_WINDOW_CLOSED;
				DispatchEvent(event);

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