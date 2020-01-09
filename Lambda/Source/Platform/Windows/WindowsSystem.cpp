#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Engine/Engine.h"
	#include "Core/Event/SystemEvent.h"

	#include "Platform/Windows/WindowsSystem.h"
	#include "Platform/Windows/WindowsWindow.h"
    #include "Platform/Windows/WindowsKeyboard.h"

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	//-------------
	//WindowsSystem
	//-------------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
    ISystem* WindowsSystem::Create()
    {
        //Setup debugflags
#if defined(LAMBDA_DEBUG)
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
        ISystem* pSystem = DBG_NEW WindowsSystem();
        return pSystem;
    }

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
    WindowsSystem::WindowsSystem()
		: ISystem()
	{
		//Init other members
		m_hInstance = static_cast<HINSTANCE>(GetModuleHandle(0));

        //Initialize lookuptable for keycodes
        WindowsKeyboard::Initialize();

		//Register WindowClass
		RegisterWindowClass(m_hInstance);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
    WindowsSystem::~WindowsSystem()
	{
		m_hInstance = 0;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsSystem::RegisterWindowClass(HINSTANCE hInstance)
	{
		//Register window class
		WNDCLASSEX wc = {};
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WindowsWindow::MessageProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WindowsWindow::WindowClass();
		wc.hIconSm = 0;

		::RegisterClassEx(&wc);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	IWindow* WindowsSystem::CreateWindow(const char* pTitle, uint32 width, uint32 height)
	{
		//Create window
		WindowsWindow* pWindow = DBG_NEW WindowsWindow(this);
		if (!pWindow->Init(pTitle, width, height))
		{
			return nullptr;
		}

		return pWindow;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void WindowsSystem::ProcessEvents()
	{
		MSG message = {};
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			//When we recive a quit message we exit the engine
			if (message.message == WM_QUIT)
			{
                Engine& engine = Engine::Get();
                engine.Exit(int32(message.wParam));
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	LRESULT WindowsSystem::OnMessage(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam)
	{
        switch (message)
        {
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }

            case WM_SIZE:
            {
                SystemEvent event = {};
                event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_RESIZED;
                event.WindowResizedEvent.Width  = uint32(LOWORD(lParam));
                event.WindowResizedEvent.Height = uint32(HIWORD(lParam));

                if (wParam == SIZE_MAXIMIZED)
                    event.WindowResizedEvent.ResizeType = EResizeType::RESIZE_TYPE_MAXIMIZED;
                else if (wParam == SIZE_MINIMIZED)
                    event.WindowResizedEvent.ResizeType = EResizeType::RESIZE_TYPE_MINIMIZED;
                else
                    event.WindowResizedEvent.ResizeType = EResizeType::RESIZE_TYPE_UNKNOWN;

                DispatchEvent(event);
                return 0;
            }

            case WM_MOVE:
            {
                //Send event
                SystemEvent event = {};
                event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_MOVED;
                event.WindowMovedEvent.x = uint32(LOWORD(lParam));
                event.WindowMovedEvent.y = uint32(HIWORD(lParam));

                DispatchEvent(event);
                return 0;
            }

            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                SystemEvent event = {};
                event.EventType = (message == WM_KEYUP) ? ESystemEvent::SYSTEM_EVENT_KEY_RELEASED : ESystemEvent::SYSTEM_EVENT_KEY_PRESSED;
                event.KeyEvent.Key          = WindowsKeyboard::ConvertVirtualKey(uint32(wParam));
                event.KeyEvent.Modifiers    = WindowsKeyboard::GetModifierKeys();
                event.KeyEvent.RepeatCount  = uint32(LOWORD(lParam));

                DispatchEvent(event);
                return 0;
            }

            case WM_CHAR:
            {
                SystemEvent event = {};
                event.EventType = ESystemEvent::SYSTEM_EVENT_KEY_TEXT;
                event.KeyTextEvent.Character = uint32(wParam);

                DispatchEvent(event);
                return 0;
            }

            case WM_MOUSEMOVE:
            {
                SystemEvent event = {};
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

                SystemEvent event = {};
                event.MouseButtonEvent.Button    = button;
                event.MouseButtonEvent.Modifiers = WindowsKeyboard::GetModifierKeys();
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

                SystemEvent event = {};
                event.EventType = ESystemEvent::SYSTEM_EVENT_MOUSE_SCROLLED;
                event.MouseScrolledEvent.Horizontal = horizontalValue;
                event.MouseScrolledEvent.Vertical   = verticalValue;

                DispatchEvent(event);
                return 0;
            }

            case WM_SETFOCUS:
            case WM_KILLFOCUS:
            {
                //Send event
                SystemEvent event = {};
                event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_FOCUS_CHANGED;
                event.WindowFocusChangedEvent.bHasFocus = message == WM_SETFOCUS;

                DispatchEvent(event);
                return 0;
            }
        }

        //Handle messages not handled
        return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
#endif