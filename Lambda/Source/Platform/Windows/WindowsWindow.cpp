#include "LambdaPch.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "System/Log.h"
	#include "Utilities/StringHelper.h"
	#include "Events/KeyEvent.h"
	#include "Events/WindowEvent.h"
	#include "Events/MouseEvent.h"
	#include "WindowsHelper.h"
	#include "WindowsWindow.h"
	#include "WindowClass.h"
	#include "WindowsInput.h"
	#include "../Vulkan/VKNDevice.h"
	#include "../DX12/DX12Device.h"
	#define	NAME_APPWINDOW L"AppWindow"

namespace Lambda
{
	//-------
	//IWindow
	//-------

	IWindow* IWindow::Create(const WindowDesc& desc)
	{
		return DBG_NEW WindowsWindow(desc);
	}

	//-------------
	//WindowsWindow
	//-------------

	LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	WindowsWindow::WindowsWindow(const WindowDesc& desc)
		: m_pEventCallback(nullptr),
		m_hWindow(0),
		m_Fullscreen(false),
		m_HasFocus(false)
	{
		Init(desc);
		LOG_SYSTEM_INFO("[LAMBDA ENGINE] Created window. w: %d, h: %d\n", desc.Width, desc.Height);
	}
	

	WindowsWindow::~WindowsWindow()
	{
		SetFullscreen(false);

		SafeDelete(m_pEventCallback);
		if (IsWindow(m_hWindow))
		{
			DestroyWindow(m_hWindow);
			m_hWindow = 0;

			WindowClass::Unregister(NAME_APPWINDOW);
		}
	}
	

	void WindowsWindow::SetEventCallback(IEventCallback* pCallback)
	{
		if (m_pEventCallback)
		{
			SafeDelete(m_pEventCallback);
		}
		m_pEventCallback = pCallback;
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


	bool WindowsWindow::HasFocus() const
	{
		return m_HasFocus;
	}


	void* WindowsWindow::GetNativeHandle() const
	{
		return reinterpret_cast<void*>(m_hWindow);
	}


	uint32 WindowsWindow::GetHeight() const
	{
		return m_Height;
	}


	uint32 WindowsWindow::GetWidth() const
	{
		return m_Width;
	}


	bool WindowsWindow::SetFullscreen(bool fullscreen)
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
				m_Style		= GetWindowLongW(m_hWindow, GWL_STYLE);
				m_ExStyle	= GetWindowLongW(m_hWindow, GWL_EXSTYLE);

				SetWindowLongW(m_hWindow, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
				SetWindowLongW(m_hWindow, GWL_EXSTYLE, WS_EX_APPWINDOW);
				SetWindowPos(m_hWindow, HWND_TOP, 0, 0, m_Width, m_Height, SWP_FRAMECHANGED);
				ShowWindow(m_hWindow, SW_SHOWMAXIMIZED);

				InvalidateRect(m_hWindow, 0, TRUE);
                
                m_Fullscreen = true;

				LOG_DEBUG_WARNING("[LAMBDA ENGINE] Switched into fullscreen\n");
			}
			else 
			{
				LOG_DEBUG_ERROR("[LAMBDA ENGINE] Failed to switch into fullscreen\n");
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

				LOG_DEBUG_INFO("[LAMBDA ENGINE] Switched into windowed mode\n");
			}
            else
            {
				LOG_DEBUG_ERROR("[LAMBDA ENGINE] Failed to switch from fullscreen\n");
                return false;
            }
		}

		return true;
	}


	bool WindowsWindow::GetFullscreen() const
	{
		return m_Fullscreen;
	}


	void WindowsWindow::Init(const WindowDesc& desc)
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

			WindowClass::Register(wc);

			//Set client area size
			m_Style		= WS_OVERLAPPEDWINDOW;
			m_ExStyle	= 0;
			RECT rect	= { 0, 0, static_cast<LONG>(desc.Width), static_cast<LONG>(desc.Height) };
			AdjustWindowRect(&rect, m_Style, false);

            
            //Make sure name is not nullptr
            const char* pName = desc.pTitle;
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
				m_Width	 = desc.Width;
				m_Height = desc.Height;

				//Set userdata so we can retrive this-pointer when handling events
				SetWindowLongPtr(m_hWindow, GWLP_USERDATA, reinterpret_cast<uintptr_t>(this));
				ShowWindow(m_hWindow, SW_NORMAL);
			}
		}

		//Set fullscreen
		SetFullscreen(desc.Fullscreen);
	}


	void WindowsWindow::DispatchEvent(const Event& event)
	{
		if (m_pEventCallback)
			m_pEventCallback->Callback(event);
	}


	LRESULT WindowsWindow::OnEvent(uint32 msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_DESTROY:
		{
			WindowClosedEvent event = WindowClosedEvent();
			DispatchEvent(event);
			break;
		}
		
		case WM_SIZE:
		{
			WindowResizeEvent event = WindowResizeEvent(uint32(LOWORD(lParam)), uint32(HIWORD(lParam)));
			DispatchEvent(event);
			break;
		}

		case WM_MOVE:
		{
			WindowMoveEvent event = WindowMoveEvent(uint32(LOWORD(lParam)), uint32(HIWORD(lParam)));
			DispatchEvent(event);
			break;
		}
		
		case WM_KEYDOWN:
		{
			KeyPressedEvent event = KeyPressedEvent(WindowsInput::ConvertWindowsKey(uint32(wParam)), GetKeyModifers(), uint32(LOWORD(lParam)));
			DispatchEvent(event);
			break;
		}
		case WM_KEYUP:
		{
			KeyReleasedEvent event = KeyReleasedEvent(WindowsInput::ConvertWindowsKey(uint32(wParam)), Lambda::GetKeyModifers());
			DispatchEvent(event);
			break;
		}

		case WM_CHAR:
		{
			KeyTypedEvent event = KeyTypedEvent(uint32(wParam));
			DispatchEvent(event);
			break;
		}

		case WM_MOUSEMOVE:
		{
			MouseMovedEvent event = MouseMovedEvent(uint32(GET_X_LPARAM(lParam)), uint32(GET_Y_LPARAM(lParam)));
			DispatchEvent(event);
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_XBUTTONDOWN:
		{
			MouseButtonPressedEvent event = MouseButtonPressedEvent(WindowsInput::ConvertWindowsButton(uint32(wParam)), Lambda::GetKeyModifers());
			DispatchEvent(event);
			break;
		}

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_XBUTTONUP:
		{
			MouseButtonReleasedEvent event = MouseButtonReleasedEvent(WindowsInput::ConvertWindowsButton(uint32(wParam)), Lambda::GetKeyModifers());
			DispatchEvent(event);
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

			MouseScrolledEvent event = MouseScrolledEvent(horizontalValue, verticalValue);
			DispatchEvent(event);
			break;
		}

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
			//Update has focus
			m_HasFocus = msg == WM_SETFOCUS;

			//Send event to rest of the system
			WindowFocusChangedEvent event = WindowFocusChangedEvent(m_HasFocus);
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
		WindowsWindow* pWindow = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (pWindow)
			return pWindow->OnEvent(msg, wParam, lParam);
		else
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}
#endif
