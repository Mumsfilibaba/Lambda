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

        LOG_DEBUG_INFO("Lambda Engine: STARTING\n");
        
		Clock clock;
		Time accumulator;
		const Time timestep = Time::Seconds(1.0f / 60.0f);
		uint32 fps = 0;
		uint32 ups = 0;

		clock.Reset();
		while (m_Running)
		{
			clock.Tick();
            
			//Logic update
			accumulator += clock.GetDeltaTime();
			while (accumulator >= timestep)
            {
				InternalOnUpdate(timestep);
				accumulator -= timestep;
                ups++;
			}

			//Render when the application has focus
            if (m_HasFocus)
            {
                InternalOnRender(clock.GetDeltaTime());
                fps++;
            }
			
			//Print FPS and UPS to console
			if (clock.GetTotalTime().AsSeconds() >= 1.0f)
			{
				clock.Reset();
				LOG_SYSTEM_PRINT("FPS: %u, UPS: %u\n", fps, ups);
				fps = 0;
				ups = 0;
			}
		}

        LOG_DEBUG_INFO("Lambda Engine: EXITING\n");
        
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
            desc.Api    = GRAPHICS_API_VULKAN;
#if LAMBDA_DEBUG
            desc.Flags  = GRAPHICS_CONTEXT_FLAG_DEBUG;
#else
            desc.Flags  = GRAPHICS_CONTEXT_FLAG_NONE;
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
        //Window update (Handle events)
        m_pWindow->OnUpdate();
        
        //Update controllers
		JoystickManager::OnUpdate();
		
        //Call update 
        OnUpdate(dt);
	}


	void Application::InternalOnRender(Time dt)
	{
		OnRender(dt);
	}


	void Application::InternalOnRelease()
	{
		OnRelease();

        //Destroy graphics context
        if (m_pGraphicsContext)
        {
            m_pGraphicsContext->Destroy();
            m_pGraphicsContext = nullptr;
        }
        
        //Destroy window
        SafeDelete(m_pWindow);
	}


	bool Application::OnEvent(const Event& event)
	{
        switch(event.Type)
        {
            //Exit the application when the window is closed
            case EVENT_TYPE_WINDOW_CLOSED:
                Application::GetInstance().Quit(0);
                LOG_DEBUG_INFO("Window closed\n");
                return true;
            
            //Exit the application when the escape key is pressed
            case EVENT_TYPE_KEYDOWN:
                if (event.KeyEvent.KeyCode == KEY_ESCAPE)
                {
                    Application::GetInstance().Quit(0);
                    LOG_DEBUG_INFO("Escape pressed, exiting\n");
                    return true;
                }
                else
                {
                    return false;
                }
                
            //When window is out of focus make sure that the rendering-loop pauses
            case EVENT_TYPE_FOCUS_CHANGED:
                IGraphicsDevice::GetInstance()->WaitForGPU();
                GetInstance().m_HasFocus = event.FocusChanged.HasFocus;
                
                //Return false so that other parts of the app still can get this event
                return false;
            
            //If not an handled event return false to let it continue in the eventhandler
            default:
                return false;
        }
	}
}
