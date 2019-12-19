#include "LambdaPch.h"

#include "Core/CEngine.h"
#include "Core/CLayer.h"
#include "Core/CLayerStack.h"
#include "Core/CLogManager.h"

#include "Time/CClock.h"

#include "Core/Event/EventCallback.h"
#include "Core/Event/SWindowEvent.h"
#include "Core/Event/SKeyEvent.h"
#include "Core/Event/SMouseEvent.h"

#include "Core/Input/CMouse.h"
#include "Core/Input/CGamepad.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/CMouseController.h"
#include "Core/Input/CKeyboardController.h"

#if defined(LAMBDA_PLAT_WINDOWS)
#include "../Platform/Windows/WindowsHelper.h"
#include "../Platform/Windows/CWindowsWindow.h"
#include "../Platform/Windows/CXinputGamepadController.h"

//--------------------------------------------------------
//Advise graphics drivers to use discrete GPU on notebooks
//--------------------------------------------------------

extern "C"
{
	// nvidia
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	// AMD
	_declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

//--------------------------------------------------------------------------------------------------------
//_CreateGameLayer - Function pointer to create the gamelayer - Needed for DLL- and Static-Lib compilation
//--------------------------------------------------------------------------------------------------------

LAMBDA_API Lambda::CLayer* (*_CreateGameLayer)(void) = nullptr;

namespace Lambda
{
	//----------
	//LambdaMain
	//----------

	int32 LambdaMain(const SEngineParams& params)
	{
#if defined(LAMBDA_DEVELOP) && defined(LAMBDA_PLAT_WINDOWS)
		//Enable debug memory leakchecker
		DBG_MEMLEAK_CHECK();
		
		//We only initialize the console on windows since this function is empty on other platforms
		if (LInitializeConsole() == false)
		{
			return -1;
		}
#endif

		//Initialize lookup tables
		LInitializeKeysLookupTable();

		//Create engine
		CEngine* pEngine = CreateEngineInterface(params);

		int32 exitCode = 0;
		if (pEngine)
		{
			pEngine->RunMainLoop();
			exitCode = pEngine->GetExitCode();

			pEngine->Release();
		}
		else
		{
			exitCode = -1;
		}
        
		//We only release the console on windows since this function is empty on other platforms
#if defined(LAMBDA_DEVELOP) && defined(LAMBDA_PLAT_WINDOWS)
        LReleaseConsole();
#endif
		return exitCode;
	}

	//---------------------
	//CreateEngineInterface
	//---------------------

	CEngine* CreateEngineInterface(const SEngineParams& params)
	{
		CEngine* pLEngine = DBG_NEW CEngine();
		if (pLEngine->Initialize(params))
		{
			return pLEngine;
		}

		return nullptr;
	}

	//-------
	//CEngine
	//-------

	template<>
	CEngine* CSingleton<CEngine>::s_pInstance = nullptr;

	CEngine::CEngine()
		: CSingleton<CEngine>(),
		m_pEnvironment(nullptr),
		m_pLogManager(nullptr),
		m_pEventDispatcher(nullptr),
		m_pMouseController(nullptr),
		m_pGamepadController(nullptr),
		m_pKeyboardController(nullptr),
		m_LayerStack(),
		m_FrameClock(),
		m_FrameAccumulator(),
		m_Timestep(CTime::Seconds(1.0f / 60.0f)),
		m_IsRunning(true),
		m_ExitCode(0)
	{
	}


	void CEngine::Release()
	{
		//Release input controllers
		SafeDelete(m_pMouseController);
		SafeDelete(m_pGamepadController);
		SafeDelete(m_pKeyboardController);

		m_LayerStack.ReleaseLayers();
		m_pEventDispatcher->Release();
		m_pEnvironment->Release();
		m_pLogManager->Release();
		delete this;
	}

	
	bool CEngine::Initialize(const SEngineParams&)
	{
		//Init LogManager
		m_pLogManager = DBG_NEW CLogManager();

		//Create eventdipatcher
		m_pEventDispatcher = DBG_NEW CEventDispatcher();
		
		//Create controller for keyboard
		IKeyboardController* pKeyboardController = DBG_NEW CKeyboardController();
		SetKeyboardController(pKeyboardController);

		//Create controller for mouse
		IMouseController* pMouseController = DBG_NEW CMouseController();
		SetMouseController(pMouseController);

		//Create controller for gamepad
#if defined(LAMBDA_PLAT_WINDOWS)
		IGamepadController* pGamepadController = DBG_NEW CXInputGamepadController();
		SetGamepadController(pGamepadController);
#else
		CDummyGamepadController* pGamepadController = DBG_NEW CDummyGamepadController();
		SetGamepadController(pGamepadController);
#endif

#if defined(LAMBDA_DESKTOP)
		//Create window on desktop
		m_pWindow = IWindow::CreateWindow("Lambda Engine", 1440, 900);
#endif

		//Make sure that the Create game layer function is set then create gamelayer
		if (_CreateGameLayer == nullptr)
		{
			return false;
		}

		CLayer* pGameLayer = _CreateGameLayer();
        m_LayerStack.PushLayer(pGameLayer);
		return true;
	}


	void CEngine::RunMainLoop()
	{
		LOG_ENGINE_INFO("Starting MainLoop\n");

		//Reset clock before starting the loop or we get inaccurate timings
		m_FrameClock.Reset();

		//Start loop
		while (m_IsRunning)
		{
			m_pEnvironment->ProcessEvents();         
			DoFrame();
		}

		LOG_ENGINE_INFO("Ending MainLoop\n");
	}

	
	void CEngine::ProcessEvents()
	{
#if defined(LAMBDA_PLAT_WINDOWS)
		MSG msg = {};
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			//Quit application
			if (msg.message == WM_QUIT)
			{
				Exit((int32)msg.wParam);
			}
		}
#else

#endif
	}


	void CEngine::DoFrame()
	{
		static uint32 ups = 0;

		//Logic update
		m_FrameClock.Tick();
		m_FrameAccumulator += m_FrameClock.GetDeltaTime();
		while (m_FrameAccumulator >= m_Timestep)
		{
			//Update input at the same rate as update
			CMouse::UpdateState();
			CGamepad::UpdateState();
			CKeyboard::UpdateState();

			//Update all layers
			for (auto pLayer : m_LayerStack)
			{
				pLayer->OnUpdate(m_Timestep);
			}

			m_FrameAccumulator -= m_Timestep;
			ups++;
		}

		//Print UPS to console
		if (m_FrameClock.GetTotalTime().AsSeconds() >= 1.0f)
		{
			LOG(LOG_CHANNEL_ENGINE, LOG_SEVERITY_INFO, "UPS: %u, Frametime: %.4fms\n", ups, m_FrameClock.GetDeltaTime().AsMilliSeconds());

			m_FrameClock.Reset();
			ups = 0;
		}
	}

	
	void CEngine::Exit(int32 exitCode)
	{
		m_ExitCode = exitCode;
		m_IsRunning = false;
	}

	
	void CEngine::SetMouseController(IMouseController* pMouseController)
	{
		LAMBDA_ASSERT_PRINT(pMouseController != nullptr, "pMouseController cannot be nullptr");

		SafeDelete(m_pMouseController);
		m_pMouseController = pMouseController;
	}


	void CEngine::SetKeyboardController(IKeyboardController* pKeyboardController)
	{
		LAMBDA_ASSERT_PRINT(pKeyboardController != nullptr, "pKeyboardController cannot be nullptr");

		SafeDelete(m_pKeyboardController);
		m_pKeyboardController = pKeyboardController;
	}


	void CEngine::SetGamepadController(IGamepadController* pGamepadController)
	{
		LAMBDA_ASSERT_PRINT(pGamepadController != nullptr, "pGamepadController cannot be nullptr");

		SafeDelete(m_pGamepadController);
		m_pGamepadController = pGamepadController;
	}


#if defined(LAMBDA_PLAT_WINDOWS)
	LRESULT CEngine::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//Send event to window
		CWindowsWindow* pWindow = reinterpret_cast<CWindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (pWindow)
		{
			pWindow->OnEvent(msg, wParam, lParam);
		}

		//Handle events
		switch (msg)
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
			float horizontalValue	= 0.0f;
			float verticalValue		= 0.0f;
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


	LRESULT CALLBACK MessageCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		return CEngine::GetPtr()->HandleMessage(hWnd, msg, wParam, lParam);
	}
#endif
}
