#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Event/SystemEvent.h"	

	#include "Platform/Windows/WindowsApplication.h"
	#include "Platform/Windows/WindowsKeyboard.h"
	#include "Platform/Windows/WindowsWindow.h"

namespace Lambda
{
	//-------------------
	//CWindowsApplication
	//-------------------

	HINSTANCE CWindowsApplication::s_hInstance = 0;
	CWindowsApplication* CWindowsApplication::s_pWindowsApplication = nullptr;

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    CApplication* CWindowsApplication::CreateApplication()
    {
        s_pWindowsApplication = DBG_NEW CWindowsApplication();
        return s_pWindowsApplication;
    }

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CWindowsApplication::CWindowsApplication()
		: CApplication(),
        m_Windows()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CWindowsApplication::~CWindowsApplication()
	{
        //Release all windows
        for (auto window : m_Windows)
            window.second->Release();

        m_Windows.clear();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsApplication::RegisterWindowClass(HINSTANCE hInstance)
	{
		//Register window class
		WNDCLASSEX wc = {};
		wc.cbSize			= sizeof(WNDCLASSEX);
		wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc		= CWindowsApplication::WndProc;
		wc.cbClsExtra		= 0;
		wc.cbWndExtra		= 0;
		wc.hInstance		= hInstance;
		wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground	= reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName		= NULL;
		wc.lpszClassName	= CWindowsWindow::WindowClass();
		wc.hIconSm = 0;

		::RegisterClassEx(&wc);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CWindow* CWindowsApplication::GetCurrentWindow() const
	{
        HWND hWnd = ::GetForegroundWindow();
		return FindWindowFromHWND(hWnd);
	}

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CWindowsApplication::Terminate(int32 exitCode) const
    {
        ::PostQuitMessage(exitCode);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CWindow* CWindowsApplication::CreateWindow(const SWindowProps& props)
	{
		LAMBDA_ASSERT_PRINT(s_pWindowsApplication, "CWindowsApplication not initialized");

        CWindowsWindow* pWindowsWindow = DBG_NEW CWindowsWindow(s_pWindowsApplication);
        if (pWindowsWindow->Init(props))
        {
            HWND hWnd = pWindowsWindow->GetHandle();
            m_Windows.insert({ hWnd, pWindowsWindow });
		    return pWindowsWindow;
        }

        return nullptr;
	}

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    CWindowsWindow* CWindowsApplication::FindWindowFromHWND(HWND hWnd) const
    {
        auto window = m_Windows.find(hWnd);
        if (window != m_Windows.end())
            return window->second;

        return nullptr;
    }

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsApplication::PreInit(HINSTANCE hInstance)
	{
		s_hInstance = hInstance;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsApplication::Init()
	{
		CWindowsKeyboard::Init();
		RegisterWindowClass(s_hInstance);
	}

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void CWindowsApplication::Release()
    {
        s_pWindowsApplication = nullptr;
    }

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CWindowsApplication::ProcessEvents()
	{
		MSG message = {};
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			//When we recive a quit message we exit the engine
			if (message.message == WM_QUIT)
			{
				SSystemEvent event = {};
                event.EventType = ESystemEvent::SYSTEM_EVENT_APP_EXIT;
				event.AppExitEvent.ExitCode = 0;

				s_pWindowsApplication->DispatchEvent(event);
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LRESULT CWindowsApplication::WndProc(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam)
	{
        if (s_pWindowsApplication)
            return s_pWindowsApplication->HandleMessage(hWnd, message, wParam, lParam);
        else
            return DefWindowProc(hWnd, message, wParam, lParam);
	}

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LRESULT CWindowsApplication::HandleMessage(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam)
	{
        //Let window process message
        CWindowsWindow* pWindow = FindWindowFromHWND(hWnd);
        pWindow->ProcessMessage(message, wParam, lParam);

        //Process event
        SSystemEvent event = {};
        switch (message)
        {
            case WM_DESTROY:
            {
                Terminate(0);
                return 0;
            }

            case WM_SIZE:
            {
                event.EventType = ESystemEvent::SYSTEM_EVENT_DISPLAY_RESIZED;
                event.DisplayResizeEvent.Width  = uint32(LOWORD(lParam));
                event.DisplayResizeEvent.Height = uint32(HIWORD(lParam));

                if (wParam == SIZE_MAXIMIZED)
                    event.DisplayResizeEvent.ResizeType = EResizeType::RESIZE_TYPE_MAXIMIZED;
                else if (wParam == SIZE_MINIMIZED)
                    event.DisplayResizeEvent.ResizeType = EResizeType::RESIZE_TYPE_MINIMIZED;
                else
                    event.DisplayResizeEvent.ResizeType = EResizeType::RESIZE_TYPE_UNKNOWN;

                DispatchEvent(event);
                return 0;
            }

            case WM_MOVE:
            {
                event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_MOVE;
                event.WindowMovedEvent.x = uint32(LOWORD(lParam));
                event.WindowMovedEvent.y = uint32(HIWORD(lParam));

                DispatchEvent(event);
                return 0;
            }

            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                event.EventType = (message == WM_KEYUP) ? ESystemEvent::SYSTEM_EVENT_KEY_RELEASED : ESystemEvent::SYSTEM_EVENT_KEY_PRESSED;
                event.KeyEvent.Key          = CWindowsKeyboard::ConvertVirtualKey(uint32(wParam));
                event.KeyEvent.Modifiers    = CWindowsKeyboard::GetModifierKeys();
                event.KeyEvent.RepeatCount  = uint32(LOWORD(lParam));

                DispatchEvent(event);
                return 0;
            }

            case WM_CHAR:
            {
                event.EventType = ESystemEvent::SYSTEM_EVENT_KEY_TEXT;
                event.KeyTextEvent.Character = uint32(wParam);

                DispatchEvent(event);
                return 0;
            }

            case WM_MOUSEMOVE:
            {
                event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_MOVED;
                event.MouseMovedEvent.x = uint32(GET_X_LPARAM(lParam));
                event.MouseMovedEvent.y = uint32(GET_Y_LPARAM(lParam));

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

                event.MouseButtonEvent.Button    = button;
                event.MouseButtonEvent.Modifiers = CWindowsKeyboard::GetModifierKeys();
                if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP || message == WM_XBUTTONUP)
                {
                    //When the mousebutton is released we also release the mousecapture
                    ::ReleaseCapture();
                    event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_BUTTON_RELEASED;
                }
                else
                {
                    //Mouse is down so we capture the mouse
                    ::SetCapture(hWnd);
                    event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_BUTTON_PRESSED;
                }

                DispatchEvent(event);
                return 0;
            }

            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
            {
                float value = float(GET_WHEEL_DELTA_WPARAM(wParam)) / float(WHEEL_DELTA);
                float horizontalValue = 0.0f;
                float verticalValue = 0.0f;
                if (message == WM_MOUSEHWHEEL)
                {
                    horizontalValue = value;
                }
                else
                {
                    verticalValue = value;
                }

                event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_SCROLLED;
                event.MouseScrolledEvent.Horizontal = horizontalValue;
                event.MouseScrolledEvent.Vertical   = verticalValue;

                DispatchEvent(event);
                return 0;
            }

            case WM_SETFOCUS:
            case WM_KILLFOCUS:
            {
                event.EventType = ESystemEvent::SYSTEM_EVENT_APP_FOCUS_CHANGED;
                event.AppFocusChangedEvent.bHasFocus = (message == WM_SETFOCUS);

                DispatchEvent(event);
                return 0;
            }
        }

        //Handle messages not handled
        return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
#endif