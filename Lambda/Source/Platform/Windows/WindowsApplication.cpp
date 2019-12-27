#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
    #include "Platform/Windows/WindowsApplicattion.h"
    #include "Platform/Windows/CWindowsWindow.h"

namespace Lambda
{
    namespace WindowsApplication
    {
        //-------------------
        //SWindowsApplication
        //-------------------
        struct SWindowsApplication
        {
            CWindowsWindow* pWindow = nullptr;
        };
        SWindowsApplication g_WindowsApplication;


        //------------------
        //WindowsApplication
        //------------------
		void InitializeKeycodeLookupTable();

        bool Initialize()
        {
            InitializeKeycodeLookupTable();

            //Create window
            CWindowsWindow* pWindow = DBG_NEW CWindowsWindow("Lambda Engine", 1440, 900);
            g_WindowsApplication.pWindow = pWindow;
        }

        
        void ProcessEvents()
        {
            MSG message = {};
            while (PeekMessage(&message, 0, 0, 0, PM_REMOVE) > 0)
            {
                TranslateMessage(&message);
                DispatchMessage(&message);

                if (message.message == WM_QUIT)
                {
                    Engine::Exit(int32(message.wParam));
                    break;
                }
            }
        }


        void Release()
        {
            SafeDelete(g_WindowsApplication.pWindow);
        }


        void SetEventCallback(const CEventCallback& eventCallback)
        {

        }
        

        IWindow* GetNativeWindow();
        {
            return g_WindowsApplication.pWindow;
        }


        LRESULT CALLBACK MessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            //Send event to window
            CWindowsWindow* pWindow = reinterpret_cast<CWindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
            if (pWindow)
            {
                pWindow->OnEvent(msg, wParam, lParam);
            }

            //Handle events
            switch (message)
            {
            case WM_DESTROY:
            {
                if (pWindow)
                {
                    if (pWindow->GetHWND() == hWnd)
                    {
                        PostQuitMessage(0);
                    }
                }

                return 0;
            }

            case WM_SIZE:
            {
                CWindowResizeEvent event = CWindowResizeEvent(uint32(LOWORD(lParam)), uint32(HIWORD(lParam)));
                m_pEventDispatcher->DispatchEvent(event);
                break;
            }

            case WM_MOVE:
            {
                CWindowMoveEvent event = CWindowMoveEvent(uint32(LOWORD(lParam)), uint32(HIWORD(lParam)));
                m_pEventDispatcher->DispatchEvent(event);
                break;
            }
            case WM_KEYDOWN:
            {
                EKey key = LConvertKeyFromSystem(uint32(wParam));

                CKeyPressedEvent event = CKeyPressedEvent(key, GetKeyModifers(), uint32(LOWORD(lParam)));
                m_pEventDispatcher->DispatchEvent(event);
                break;
            }
            case WM_KEYUP:
            {
                EKey key = LConvertKeyFromSystem(uint32(wParam));

                CKeyReleasedEvent event = CKeyReleasedEvent(key, GetKeyModifers());
                m_pEventDispatcher->DispatchEvent(event);
                break;
            }

            case WM_CHAR:
            {
                CKeyTypedEvent event = CKeyTypedEvent(uint32(wParam));
                m_pEventDispatcher->DispatchEvent(event);
                break;
            }

            case WM_MOUSEMOVE:
            {
                CMouseMovedEvent event = CMouseMovedEvent(uint32(GET_X_LPARAM(lParam)), uint32(GET_Y_LPARAM(lParam)));
                m_pEventDispatcher->DispatchEvent(event);
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

                    //Send event
                    CMouseButtonReleasedEvent event = CMouseButtonReleasedEvent(button, modifiers);
                    m_pEventDispatcher->DispatchEvent(event);
                }
                else
                {
                    //Mouse is down so we capture the mouse
                    SetCapture(pWindow->GetHWND());

                    //Set event
                    CMouseButtonPressedEvent event = CMouseButtonPressedEvent(button, modifiers);
                    m_pEventDispatcher->DispatchEvent(event);
                }
                break;
            }

            case WM_MOUSEWHEEL:
            case WM_MOUSEHWHEEL:
            {
                float value = float(GET_WHEEL_DELTA_WPARAM(wParam)) / float(WHEEL_DELTA);
                float horizontalValue = 0.0f;
                float verticalValue = 0.0f;
                if (msg == WM_MOUSEHWHEEL)
                    horizontalValue = value;
                else
                    verticalValue = value;

                CMouseScrolledEvent event = CMouseScrolledEvent(horizontalValue, verticalValue);
                m_pEventDispatcher->DispatchEvent(event);
                break;
            }

            case WM_SETFOCUS:
            case WM_KILLFOCUS:
            {
                CWindowFocusChangedEvent event = CWindowFocusChangedEvent(msg == WM_SETFOCUS);
                m_pEventDispatcher->DispatchEvent(event);
                break;
            }

            default:
                return DefWindowProc(hWnd, msg, wParam, lParam);
            }

            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }
}
#endif