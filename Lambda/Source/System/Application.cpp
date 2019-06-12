#include "LambdaPch.h"
#include "System/Application.h"
#include "System/Clock.h"
#include "System/Log.h"
#include "System/Input.h"
#include "System/JoystickManager.h"

namespace Lambda
{
	Application* Application::s_pInstance = nullptr;

	Application::Application(const EngineParams& params)
		: m_pWindow(nullptr),
		m_pGraphicsContext(nullptr),
		m_Running(true)
	{
		assert(s_pInstance == nullptr);
		s_pInstance = this;

		Init(params);
	}


	int32 Application::Run()
	{
		InternalOnLoad();

		Clock clock;
		Time accumulator;
		const Time timestep = Time::Seconds(1.0f / 60.0f);
        
        LOG_DEBUG_INFO("Timestep: %u\n", timestep.AsNanoSeconds());

		uint32 fps = 0;
		uint32 ups = 0;

		clock.Reset();
		while (m_Running)
		{
            LOG_DEBUG_INFO("Ticking\n");
			clock.Tick();
			
			//Logic update
			accumulator += clock.GetDeltaTime();
			while (accumulator >= timestep)
            {
				InternalOnUpdate(timestep);
				accumulator -= timestep;
                
                LOG_DEBUG_INFO("Acc: %u\n", accumulator.AsNanoSeconds());
                
                if (accumulator < timestep)
                {
                    LOG_DEBUG_INFO("LESSER\n");
                }
                
                ups++;
			}

			//Render
			InternalOnRender(clock.GetDeltaTime());
			fps++;
			
			//Print FPS and UPS to console
			if (clock.GetTotalTime().AsSeconds() >= 1.0f)
			{
				clock.Reset();
				LOG_SYSTEM_PRINT("FPS: %u, UPS: %u\n", fps, ups);
				fps = 0;
				ups = 0;
			}
		}

        LOG_DEBUG_INFO("Application exiting\n");
        
		InternalOnRelease();
		return m_ExitCode;
	}


	IWindow* Application::GetWindow()
	{
		return m_pWindow;
	}


	void Application::Init(const EngineParams& params)
	{
		//Setup application layer
		{
			EventLayer applicationLayer = { Application::OnEvent, "ApplicationLayer" };
			EventDispatcher::PushEventLayer(applicationLayer);
		}

		//Create window
		{
			WindowDesc desc = {};
			desc.pTitle		= params.pTitle;
			desc.Width		= params.WindowWidth;
			desc.Height		= params.WindowHeight;

			m_pWindow = IWindow::Create(desc);
			m_pWindow->SetEventCallback(EventDispatcher::SendEvent);
		}

		//Create graphics context
		{
            GraphicsDeviceDesc desc;
            desc.Api = GRAPHICS_API_VULKAN;
#if LAMBDA_DEBUG
            desc.Flags = GRAPHICS_CONTEXT_FLAG_DEBUG;
#else
            desc.Flags = GRAPHICS_CONTEXT_FLAG_NONE;
#endif
			m_pGraphicsContext = IGraphicsDevice::Create(m_pWindow, desc);

			EventLayer graphicsLayer = { IGraphicsDevice::OnEvent, "GraphicsLayer" };
			EventDispatcher::PushEventLayer(graphicsLayer);
		}

		//Set joystick-pollingrate
		JoystickManager::SetPollrate(Time::Seconds(1.0f / 60.0f));
	}


	void Application::Quit(int32 exitCode)
	{
		m_Running = false;
		m_ExitCode = exitCode;
        
        LOG_DEBUG_INFO("Quit called\n");
	}


	Application& Application::GetInstance()
	{
		assert(s_pInstance != nullptr);
		return *s_pInstance;
	}
	

	void Application::InternalOnLoad()
	{
		OnLoad();
	}
	

	void Application::InternalOnUpdate(Time dt)
	{
		m_pWindow->OnUpdate();
		JoystickManager::OnUpdate();
		OnUpdate(dt);
	}


	void Application::InternalOnRender(Time dt)
	{
		OnRender(dt);
	}


	void Application::InternalOnRelease()
	{
		OnRelease();

		SafeDelete(m_pWindow);
        if (m_pGraphicsContext)
        {
            m_pGraphicsContext->Destroy();
        }
	}


	bool Application::OnEvent(const Event& event)
	{
		if (event.Type == EVENT_TYPE_WINDOW_CLOSED)
		{
			Application::GetInstance().Quit(0);
			LOG_DEBUG_INFO("Window closed\n");
			return true;
		}
		else if (event.Type == EVENT_TYPE_KEYDOWN)
		{
			if (event.KeyEvent.KeyCode == KEY_ESCAPE)
			{
				Application::GetInstance().Quit(0);
                LOG_DEBUG_INFO("Escape pressed, exiting\n");
				return true;
			}
		}

		return false;
	}
}
