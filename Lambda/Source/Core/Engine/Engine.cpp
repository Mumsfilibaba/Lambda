#include "LambdaPch.h"

#include "Core//Platform.h"

#include "Core/Input/Input.h"

#include "Core/Event/SystemEvent.h"

#include "Core/Engine/Engine.h"
#include "Core/Engine/System.h"
#include "Core/Engine/Console.h"
#include "Core/Engine/IWindow.h"


//----------------
//_CreateGameLayer
//----------------
namespace Lambda
{
	class Layer;
}

extern Lambda::Layer* (*_CreateGameLayer)();

namespace Lambda
{
	//------
	//Engine
	//------

	//Global engine instance
	Engine g_Engine;

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	Engine& Engine::Get()
	{
		return g_Engine;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Engine::RequestExit(int32 exitCode)
	{
		g_Engine.Exit(exitCode);
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
		Console::Initialize();

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
		Input::Initialize(EInputType::INPUT_TYPE_DEFAULT);

		//Create gamelayer
		LAMBDA_ASSERT_PRINT(_CreateGameLayer != nullptr, "_CreateGameLayer was nullptr");
		
		Layer* pGameLayer = _CreateGameLayer();
		PushLayer(pGameLayer);
        
		return true;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Engine::Release()
	{
		//Delete Layers
		System::RemoveListener(m_pLayerStack);

		m_pLayerStack->ReleaseLayers();
		SafeDelete(m_pLayerStack);

		//Release subsystems
		Input::Release();

		//Delete window
		SafeDelete(m_pWindow);

		//Remove as listener
		System::RemoveListener(this);
		System::Release();
		
		//Release console
		Console::Release();
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void Engine::RunMainLoop() 
	{
        Console::PrintLine("Starting up engine");
		
		//Startup engine
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
	void Engine::Exit(int32 nExitCode)
	{
		m_State.IsRunning = false;
		m_State.ExitCode  = nExitCode;
	}
}
