#include "LambdaPch.h"

#include "Core/Platform.h"
#include "Core/Input/Input.h"
#include "Core/Log/LogManager.h"
#include "Core/Event/SystemEvent.h"

#include "Core/Engine/Engine.h"
#include "Core/Engine/Console.h"

#include "Platform/Window.h"

//----------------
//_CreateGameLayer
//----------------

extern Lambda::CLayer* (*_CreateGameLayer)();

namespace Lambda
{
	//-------
	//CEngine
	//-------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CEngine* CEngine::Create()
	{
		return DBG_NEW CEngine();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	CEngine::CEngine()
		: CSingleton<CEngine>(),
        ISystemEventListener()
	{
		//Init pointer
		m_pWindow		= nullptr;
		m_pApplication	= nullptr;
		m_pLogManager	= nullptr;
		m_pLayerStack	= nullptr;

		//Setup default frametime
		m_Timestep		= CTime::Seconds(1.0f / 60.0f);
		m_UpdateBacklog = CTime(0);

		//Setup enginestate
		m_bIsRunning = false;
		m_ExitCode = 0;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CEngine::Init(const SEngineParams&)
	{
		//Create console
		Console::Attach();

		//Create logManager and log
		m_pLogManager = CLogManager::Create();
		m_pLogManager->CreateDefaultLog(ELogMode::LOG_MODE_TRUNCATE, ELogVerbosity::LOG_VERBOSITY_ERROR, true, false);

		//Create application
		CPlatformApplication::Init();
		m_pApplication = CPlatformApplication::CreateApplication();
		m_pApplication->AddEventListener(this);

		//Create CLayerStack
		m_pLayerStack = CLayerStack::Create();
		m_pApplication->AddEventListener(m_pLayerStack);

		//Create window
		SWindowProps windowProps = {};
		windowProps.pTitle	= "Lambda Engine";
		windowProps.Width	= 1440;
		windowProps.Height	= 900;

		CWindow* pWindow = m_pApplication->CreateWindow(windowProps);
		if (!pWindow)
		{
			return false;
		}
		else
		{
			m_pWindow = pWindow;
		}

		//Create inputcontroller
		Input::Init(EInputType::INPUT_TYPE_DEFAULT);

		//Print Engine info
		LOG_MESSAGE("Engine Initialized: Build=%s, Platform=%s, Architecture=%s", LAMBDA_BUILD, LAMBDA_PLATFORM, LAMBDA_ARCHITECTURE);

		//Create gamelayer		
		if (_CreateGameLayer)
		{
			D_LOG_INFO("Creating gamelayer");
			
			CLayer* pGameLayer = _CreateGameLayer();
			m_pLayerStack->PushLayer(pGameLayer);
		}
		else
		{
			D_LOG_INFO("_CreateGameLayer not defined");
		}
        
		//Show window and return
		m_pWindow->Show();
		return true;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::Release()
	{
		//Detach input
		Input::Release();

		//Delete Layers
		m_pApplication->RemoveEventListener(m_pLayerStack);
		SafeDelete(m_pLayerStack);

		//Delete application
		SafeDelete(m_pApplication);
		CPlatformApplication::Release();

		//Delete logmanager
		SafeDelete(m_pLogManager);
		
		//Detach console
		Console::Detach();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::Startup()
	{
		//Startup engine
		D_LOG_INFO("Starting up engine");
		
		m_bIsRunning = true;
		m_FrameClock.Reset();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::Tick() 
	{
		//Handle all system events
		CPlatformApplication::ProcessEvents();

		//Get deltatime
		m_FrameClock.Tick();
		CTime deltatime = m_FrameClock.GetDeltaTime();

		//Update
		if (m_UpdateBacklog >= m_Timestep)
		{
			m_pLayerStack->OnUpdate(deltatime);
			m_UpdateBacklog -= deltatime;
		}
		else
		{
			m_UpdateBacklog += deltatime;
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool CEngine::OnSystemEvent(const SSystemEvent& event)
	{
		if (event.EventType == ESystemEvent::SYSTEM_EVENT_KEY_PRESSED)
		{
			if (event.KeyEvent.Key == EKey::KEY_SPACE)
				m_pWindow->SetDisplayMode(EDisplayMode::DISPLAY_MODE_FULLSCREEN);
		}
		else if (event.EventType == ESystemEvent::SYSTEM_EVENT_APP_EXIT)
		{
			Terminate(event.AppExitEvent.ExitCode);
			return true;
		}

		return false;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void CEngine::Terminate(int32 exitCode)
	{
		//Terminating engine
		D_LOG_INFO("Terminating engine - ExitCode=%d", exitCode);

		m_bIsRunning = false;
		m_ExitCode   = exitCode;
	}
}
