#include "LambdaPch.h"

#include "Core/Engine.h"
#include "Core/Console.h"
#include "Core/Application.h"
#include "Core/CLayer.h"
#include "Core/CLayerStack.h"
#include "Core/CLogManager.h"

#include "Time/CClock.h"

#include "Core/Event/EventCallback.h"
#include "Core/Event/SWindowEvent.h"
#include "Core/Event/SKeyEvent.h"
#include "Core/Event/SMouseEvent.h"

#include "Core/Input/Input.h"
#include "Core/Input/CMouse.h"
#include "Core/Input/CGamepad.h"
#include "Core/Input/CKeyboard.h"
#include "Core/Input/CMouseController.h"
#include "Core/Input/CKeyboardController.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsHelper.h"
	#include "Platform/Windows/CWindowsWindow.h"
	#include "Platform/Windows/CXinputGamepadController.h"

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
#if defined(LAMBDA_DEVELOP)		
		//Initialize console in development-builds
		if (!Console::Initialize())
		{
			return -1;
		}
#endif
		//Initialize Engine
		int32 exitCode = 0;
		if (Engine::Initialize())
		{
			Engine::RunMainLoop();
			Engine::Release();
		}
		else
		{
			exitCode = -1;
		}
        
#if defined(LAMBDA_DEVELOP)
		//Initialize console in development-builds
		Console::Release();
#endif
		return exitCode;
	}

	//----------------
	//Engine Interface
	//----------------
	namespace Engine
	{
		//-----------
		//SSingletons
		//-----------
		struct SSingletons
		{
			CEventDispatcher* pEventDispatcher = nullptr;
			CLayerStack* pLayerStack = nullptr;
		} g_EngineSingletons;

		//--------------
		//Engine globals
		//--------------
		bool g_bIsRunning = false;
		
		//------
		//Engine
		//------
		void DoFrame(CClock& frameClock);

		bool Initialize(const SEngineParams&)
		{
			if (!Application::Initialize())
			{
				return false;
			}

			if (!Input::Initialize())
			{
				return false;
			}

			//Create eventdispatcher
			CEventDispatcher* pEventDispatcher = DBG_NEW CEventDispatcher();
			g_EngineSingletons.pEventDispatcher = pEventDispatcher;
#if defined(LAMBDA_DESKTOP)
			CEventCallback eventCallback = CEventCallback(pEventDispatcher, CEventDispatcher::OnEvent);

			IWindow* pWindow = Platform::GetNativeWindow();
			pWindow->SetEventCallback(eventCallback);
#endif			

			//Make sure that the Create game layer function is set then create gamelayer
			if (_CreateGameLayer == nullptr)
			{
				return false;
			}
			else
			{
				CLayer* pGameLayer = _CreateGameLayer();
				g_EngineGlobals.LayerStack.PushLayer(pGameLayer);
			}

			return true;
		}


		void RunMainLoop()
		{
			LOG_ENGINE_INFO("Starting MainLoop\n");

			//Reset clock before starting the loop or we get inaccurate timings
			CClock frameClock;
			frameClock.Reset();

			//Start loop
			while (g_bIsRunning)
			{
				Platform::ProcessEvents();
				DoFrame();
			}

			LOG_ENGINE_INFO("Ending MainLoop\n");
		}


		void DoFrame(CClock& frameClock)
		{
			static uint32 ups = 0;

			//Logic update
			frameClock.Tick();
			m_FrameAccumulator += m_FrameClock.GetDeltaTime();
			while (m_FrameAccumulator >= m_Timestep)
			{
				//Update input at the same rate as update
				Input::Update();

				//Update all layers
				for (auto pLayer : m_LayerStack)
				{
					pLayer->OnUpdate(m_Timestep);
				}

				m_FrameAccumulator -= m_Timestep;
				ups++;
			}

			//Print UPS to console
			if (frameClock.GetTotalTime().AsSeconds() >= 1.0f)
			{
				LOG(LOG_CHANNEL_ENGINE, LOG_SEVERITY_INFO, "UPS: %u, Frametime: %.4fms\n", ups, m_FrameClock.GetDeltaTime().AsMilliSeconds());

				frameClock.Reset();
				ups = 0;
			}
		}


		void Release()
		{
			m_LayerStack.ReleaseLayers();
			m_pEventDispatcher->Release();
			m_pLogManager->Release();
			
			Input::Release();
			Application::Release();
		}


		void Exit(int32 exitCode)
		{
			m_ExitCode = exitCode;
			m_IsRunning = false;
		}
	}

		//----------
		//Singletons
		//----------
		CLogManager* m_pLogManager;
		CEventDispatcher* m_pEventDispatcher;
		//-----------------
		//Input controllers
		//-----------------
		IMouseController*	 m_pMouseController;
		IGamepadController*  m_pGamepadController;
		IKeyboardController* m_pKeyboardController;

		CClock m_FrameClock;
		CTime m_FrameAccumulator;
		const CTime m_Timestep;
		int32 m_ExitCode;
		bool m_IsRunning;
	};
}
