#include "LambdaPch.h"
#include "Core/Engine.h"

#include "Core/Event/SEvent.h"

#if defined(LAMBDA_PLAT_WINDOWS)
    #include "Platform/Windows/CWindowsApplication.h"
    #include "Platform/Windows/CWindowsWindow.h"
    #include "Platform/Windows/WindowsHelper.h"

namespace Lambda
{
    //-------------------
    //CWindowsApplication
    //-------------------

    CWindowsApplication::CWindowsApplication(CEngine* pEngine)
        : CApplication(),
        m_pEngine(pEngine),
        m_pWindow(nullptr)
    {
    }


    CWindowsApplication::~CWindowsApplication()
    {
        SafeDelete(m_pWindow);
    }


    bool CWindowsApplication::Initialize()
    {
        InitializeKeycodeLookupTable();

        //Create window
        CWindowsWindow* pWindow = DBG_NEW CWindowsWindow("Lambda Engine", 1440, 900);
        g_WindowsApplication.pWindow = pWindow;
    }


    void CWindowsApplication::ProcessEvents()
    {
        MSG message = {};
        while (PeekMessage(&message, 0, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);

            if (message.message == WM_QUIT)
            {
                m_pEngine->Exit(int32(message.wParam));
                break;
            }
        }
    }


    LRESULT CALLBACK CWindowsApplication::MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        //Send event to window
        CWindowsWindow* pWindow = reinterpret_cast<CWindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (pWindow == nullptr)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        //Handle events
        switch (message)
        {
        case WM_DESTROY:
        {
            pWindow->OnEvent(message, wParam, lParam);
            PostQuitMessage(0);
            return 0;
        }

        case WM_SIZE:
        {
            SSystemEvent event = {};
            event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_RESIZED;
            event.WindowResizedEvent.Width = uint32(LOWORD(lParam));
            event.WindowResizedEvent.Height = uint32(HIWORD(lParam));

            if (wParam == SIZE_MAXIMIZED)
            {
                event.WindowResizedEvent.ResizeType = EResizeType::RESIZE_TYPE_MAXIMIZED;
            }
            else if (wParam == SIZE_MINIMIZED)
            {
                event.WindowResizedEvent.ResizeType = EResizeType::RESIZE_TYPE_MINIMIZED;
            }
            else
            {
                event.WindowResizedEvent.ResizeType = EResizeType::RESIZE_TYPE_UNKNOWN;
            }
            
            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
            return 0;
        }

        case WM_MOVE:
        {
            SSystemEvent event = {};
            event.EventType = ESystemEvent::SYSTEM_EVENT_WINDOW_MOVED;
            event.WindowMovedEvent.x = uint32(LOWORD(lParam));
            event.WindowMovedEvent.y = uint32(HIWORD(lParam));
            
            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
            return 0;
        }
        case WM_KEYDOWN:
        {
            SSystemEvent event = {};
            event.EventType = ESystemEvent::SYSTEM_EVENT_KEY_PRESSED;
            event.KeyEvent.Key = ;
            event.KeyEvent.Modifiers = GetKeyModifers();
            event.KeyEvent.RepeatCount = uint32(LOWORD(lParam));

            EKey key = LConvertKeyFromSystem(uint32(wParam));

            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
            return 0;
        }
        case WM_KEYUP:
        {
            EKey key = LConvertKeyFromSystem(uint32(wParam));

            CKeyReleasedEvent event = CKeyReleasedEvent(key, GetKeyModifers());
            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
            return 0;
        }

        case WM_CHAR:
        {
            CKeyTypedEvent event = CKeyTypedEvent(uint32(wParam));
            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            CMouseMovedEvent event = CMouseMovedEvent(uint32(GET_X_LPARAM(lParam)), uint32(GET_Y_LPARAM(lParam)));
            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
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

            uint32 modifiers = GetKeyModifers();
            if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP || message == WM_XBUTTONUP)
            {
                //When the mousebutton is released we also release the mousecapture
                ::ReleaseCapture();

                //Send event
                CMouseButtonReleasedEvent event = CMouseButtonReleasedEvent(button, modifiers);
                CApplication& application = CApplication::Get();
                application.DispatchEvent(event);
            }
            else
            {
                //Mouse is down so we capture the mouse
                ::SetCapture(hWnd);

                //Set event
                CMouseButtonPressedEvent event = CMouseButtonPressedEvent(button, modifiers);
                CApplication& application = CApplication::Get();
                application.DispatchEvent(event);
            }
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

            CMouseScrolledEvent event = CMouseScrolledEvent(horizontalValue, verticalValue);
            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
            return 0;
        }

        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        {
            CWindowFocusChangedEvent event = CWindowFocusChangedEvent(msg == WM_SETFOCUS);
            
            CApplication& application = CApplication::Get();
            application.DispatchEvent(event);
            return 0;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
}
#endif