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
		m_Params(),
		m_ExitCode(0),
		m_Running(true),
		m_HasFocus(false)
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
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

			
#if defined(LAMBDA_PLAT_MACOS)
            if (m_HasFocus)
            {
                //Render when the application has focus
                InternalOnRender(clock.GetDeltaTime());
                fps++;
            }
            else
            {
                //If the application does not have focus sleep for one frame (16ms)
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
#else
			//Render
			InternalOnRender(clock.GetDeltaTime());
			fps++;
#endif
			
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


	const EngineParams& Application::GetEngineParams() const
	{
		return m_Params;
	}


	IWindow* Application::GetWindow() const
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
			desc.pTitle				= params.pTitle;
			desc.Width				= params.WindowWidth;
			desc.Height				= params.WindowHeight;
			desc.GraphicsDeviceAPI	= params.GraphicsDeviceApi;
			desc.SampleCount		= params.SampleCount;
			desc.Fullscreen			= params.Fullscreen;

			m_pWindow = IWindow::Create(desc);
			m_pWindow->SetEventCallback(EventDispatcher::SendEvent);

			//Push graphics layer
			EventLayer graphicsLayer = { IGraphicsDevice::OnEvent, "GraphicsLayer" };
			EventDispatcher::PushEventLayer(graphicsLayer);
		}

		//Create ImGUI-Layer
		{
			m_pImGuiLayer = DBG_NEW ImGuiLayer();

			//Push ImGui-Layer 
			EventLayer imGuiLayer = { ImGuiLayer::OnEvent, "ImGuiLayer" };
			EventDispatcher::PushEventLayer(imGuiLayer);
		}

		//Set joystick-pollingrate
		JoystickManager::SetPollrate(Time::Seconds(1.0f / 60.0f));

		//Set Params
		m_Params = params;
	}


	void Application::Quit(int32 exitCode)
	{
		m_Running = false;
		m_ExitCode = exitCode;
        
        LOG_DEBUG_INFO("Quit called\n");
	}


	Application& Application::GetInstance()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
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
       
        //Destroy window
        SafeDelete(m_pWindow);
	}


	bool Application::InternalOnEvent(const Event& event)
	{
		switch (event.Type)
		{
			//Exit the application when the window is closed
		case EVENT_TYPE_WINDOW_CLOSED:
			Quit(0);
			LOG_DEBUG_INFO("Window closed\n");
			return true;

			//Exit the application when the escape key is pressed
		case EVENT_TYPE_KEYDOWN:
			if (event.KeyEvent.KeyCode == KEY_ESCAPE)
			{
				Quit(0);
				LOG_DEBUG_INFO("Escape pressed, exiting\n");
				return true;
			}
			else
			{
				return false;
			}

			//When window is out of focus make sure that the rendering-loop pauses
		case EVENT_TYPE_FOCUS_CHANGED:
			if (IGraphicsDevice::GetInstance())
			{
				IGraphicsDevice::GetInstance()->WaitForGPU();
			}

			m_HasFocus = event.FocusChanged.HasFocus;

			//Return false so that other parts of the app still can get this event
			return false;

			//If not an handled event return false to let it continue in the eventhandler
		default:
			return false;
		}
	}


	bool Application::OnEvent(const Event& event)
	{
		return Application::GetInstance().InternalOnEvent(event);
	}
}
