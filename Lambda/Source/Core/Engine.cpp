#include "LambdaPch.h"

#include "Core/Engine.h"
#include "Core/Console.h"
#include "Core/CLayer.h"
#include "Core/CLayerStack.h"
#include "Core/CLogManager.h"
#include "Core/CApplication.h"

#include "Time/CClock.h"

#include "Core/Event/SKeyEvent.h"
#include "Core/Event/SMouseEvent.h"
#include "Core/Event/SWindowEvent.h"
#include "Core/Event/EventCallback.h"
#include "Core/Event/CEventDispatcher.h"

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
		//Initialize input
		if (!Input::Initialize())
		{
			return false;
		}

		//Initialize Engine
		int32 exitCode = 0;
		if (CEngine::Initialize(params))
		{
			CEngine* pEngine = CEngine::GetPtr();
			pEngine->RunMainLoop();
			pEngine->Release();
			exitCode = pEngine->GetExitCode();
		}
		else
		{
			exitCode = -1;
		}
        
		//Release input
		Input::Release();
#if defined(LAMBDA_DEVELOP)
		//Initialize console in development-builds
		Console::Release();
#endif
		return exitCode;
	}

	//-------
	//CEngine
	//-------
	template<>
	CEngine* CSingleton<CEngine>::s_pInstance = nullptr;


	//Initialize engine instance
	bool CEngine::Initialize(const SEngineParams& params)
	{
		CEngine* pEngine = DBG_NEW CEngine();
		if (pEngine)
		{
			return pEngine->InternalInitialize(params);
		}

		return false;
	}


	CEngine::CEngine()
		: CSingleton(),
		m_pApplication(nullptr),
		m_pEventDispatcher(nullptr),
		m_LayerStack(),
		m_FrameTime(),
		m_bIsRunning(false)
	{
		//Init timestep
		m_FrameTime.Timestep = CTime::Seconds(1.0f / 60.0f);
	}


	CEngine::~CEngine()
	{
		m_LayerStack.ReleaseLayers();

		Input::Release();

		m_pApplication->Release();
		m_pEventDispatcher->Release();
		m_pLogManager->Release();
	}


	bool CEngine::InternalInitialize(const SEngineParams&)
	{
		//Create logmanager
		CLogManager* pLogManager = DBG_NEW CLogManager();
		m_pLogManager = pLogManager;

		//Create eventdispatcher
		CEventDispatcher* pEventDispatcher = DBG_NEW CEventDispatcher();
		m_pEventDispatcher = pEventDispatcher;

		//Create application
		CApplication* pApplication = Platform::CreateApplication();
		if (pApplication)
		{
			m_pApplication = pApplication;

			//Set event-callback
			CEventCallback eventCallback = CEventCallback(pEventDispatcher, CEventDispatcher::DispatchEvent);
			m_pApplication->SetEventCallback(eventCallback);
		}
		else
		{
			return false;
		}
	
		//Make sure that the Create game layer function is set then create gamelayer
		if (_CreateGameLayer == nullptr)
		{
			return false;
		}
		else
		{
			CLayer* pGameLayer = _CreateGameLayer();
			m_LayerStack.PushLayer(pGameLayer);
		}

		return true;
	}


	void CEngine::RunMainLoop()
	{
		LOG_ENGINE_INFO("Starting MainLoop\n");

		//Reset clock before starting the loop or we get inaccurate timings
		m_FrameTime.Clock.Reset();

		//Start loop
		while (m_bIsRunning)
		{
			m_pApplication->ProcessEvents();
			DoFrame();
		}

		LOG_ENGINE_INFO("Ending MainLoop\n");
	}


	void CEngine::DoFrame()
	{
		//Get time
		CClock& frameClock = m_FrameTime.Clock;
		CTime& deltaTime = frameClock.GetDeltaTime();
		CTime& timestep = m_FrameTime.Timestep;
		frameClock.Tick();

		//Run locig update
		m_FrameTime.Accumulator += deltaTime;
		while (m_FrameTime.Accumulator >= timestep)
		{
			//Update input at the same rate as update
			Input::Update();

			//Update all layers
			for (auto pLayer : m_LayerStack)
			{
				pLayer->OnUpdate(timestep);
			}

			m_FrameTime.Accumulator -= timestep;
			m_FrameTime.UPS++;
		}

		//Print UPS to console
		if (frameClock.GetTotalTime().AsSeconds() >= 1.0f)
		{
			LOG(LOG_CHANNEL_ENGINE, LOG_SEVERITY_INFO, "UPS: %u, Frametime: %.4fms\n", m_FrameTime.UPS, deltaTime.AsMilliSeconds());

			frameClock.Reset();
			m_FrameTime.UPS = 0;
		}
	}


	void CEngine::Release()
	{
		delete this;
	}


	void CEngine::Exit(int32 exitCode)
	{
		m_ExitCode = exitCode;
		m_bIsRunning = false;
	}
}
