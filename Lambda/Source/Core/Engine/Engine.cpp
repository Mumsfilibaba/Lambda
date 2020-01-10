#include "LambdaPch.h"

#include "Core/Platform.h"
#include "Core/Input/Input.h"
#include "Core/Log/LogManager.h"
#include "Core/Event/SystemEvent.h"

#include "Core/Engine/Engine.h"
#include "Core/Engine/System.h"
#include "Core/Engine/Console.h"
#include "Core/Engine/IWindow.h"

//----------------
//_CreateGameLayer
//----------------

extern Lambda::Layer* (*_CreateGameLayer)();

namespace Lambda
{
	//------
	//Engine
	//------

	Engine g_Engine;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Engine& Engine::Get()
	{
		return g_Engine;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Engine::Engine()
		: ISystemEventListener()
	{
		//Init pointer
		m_pWindow = nullptr;
		m_pLayerStack = nullptr;

		//Setup default frametime
		m_Frametime.Timestep	  = Time::Seconds(1.0f / 60.0f);
		m_Frametime.UpdateBacklog = Time(0);

		//Setup enginestate
		m_State.IsRunning = false;
		m_State.ExitCode = 0;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Engine::~Engine()
	{
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool Engine::Init(const EngineParams&)
	{
		//Create console
		Console::Attach();

		//Create logManager and log
		m_pLogManager = LogManager::Create();
		m_pLogManager->CreateDefaultLog(ELogMode::LOG_MODE_TRUNCATE, ELogVerbosity::LOG_VERBOSITY_ERROR, true, false);
		m_pLogManager->CreateLog("Debug", ELogMode::LOG_MODE_TRUNCATE, ELogVerbosity::LOG_VERBOSITY_ERROR, true, false);

		//Create application
		System::Initialize();
		//Add this to the system event listeners
		System::AddListener(this);

		//Create LayerStack
		m_pLayerStack = DBG_NEW LayerStack();
		System::AddListener(m_pLayerStack);

		//Create window
		IWindow* pWindow = System::CreateWindow("Lambda Engine", 1440, 900);
		if (!pWindow)
		{
			return false;
		}
		else
		{
			m_pWindow = pWindow;
		}

		//Create inputcontroller
		Input::Attach(EInputType::INPUT_TYPE_DEFAULT);

		//Create gamelayer		
		if (_CreateGameLayer)
		{
			D_LOG_INFO("Creating gamelayer");
			Layer* pGameLayer = _CreateGameLayer();
			PushLayer(pGameLayer);
		}
		else
		{
			D_LOG_INFO("_CreateGameLayer not defined");
		}
        
		return true;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Engine::Detach()
	{
		//Detach input
		Input::Detach();

		//Delete window
		SafeDelete(m_pWindow);

		//Delete Layers
		System::RemoveListener(m_pLayerStack);
		m_pLayerStack->ReleaseLayers();
		SafeDelete(m_pLayerStack);

		//Remove as listener
		System::RemoveListener(this);
		System::Detach();

		//Delete logmanager
		SafeDelete(m_pLogManager);
		
		//Detach console
		Console::Detach();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Engine::RunMainLoop() 
	{
		//Startup engine
		D_LOG_INFO("Starting up engine");
		m_State.IsRunning = true;

		//MainLoop
		m_Frametime.FrameClock.Reset();
		while (m_State.IsRunning)
		{
			//Get deltatime
			m_Frametime.FrameClock.Tick();

			//Handle all system events
			System::ProcessEvents();

			//Update
			if (m_Frametime.UpdateBacklog >= m_Frametime.Timestep)
			{
				m_pLayerStack->OnUpdate(m_Frametime.Timestep);
				m_Frametime.UpdateBacklog = Time(0);
			}
			else
			{
				m_Frametime.UpdateBacklog += m_Frametime.FrameClock.GetDeltaTime();
			}
		}

		//Terminating engine
		D_LOG_INFO("Terminating engine");
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	bool Engine::OnSystemEvent(const SystemEvent& event)
	{
		if (event.EventType == ESystemEvent::SYSTEM_EVENT_KEY_PRESSED)
		{
			if (event.KeyEvent.Key == EKey::KEY_SPACE)
			{
				m_pWindow->SetFullscreen(true);
			}
		}

		return false;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Engine::Exit(int32 exitCode)
	{
		m_State.IsRunning = false;
		m_State.ExitCode  = exitCode;

		D_LOG_INFO("Engine::Exit called, exitCode=%d", exitCode);
	}
}
