#include "LambdaPch.h"
#include "System/Application.h"
#include "Time/Clock.h"
#include "System/Log.h"
#include "System/Input.h"
#include "System/JoystickManager.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"

namespace Lambda
{
	//----------------
	//ApplicationLayer
	//----------------

	ApplicationLayer::ApplicationLayer()
		: EventLayer("ApplicationLayer")
	{
	}

	void ApplicationLayer::OnPush()
	{
	}


	void ApplicationLayer::OnPop()
	{
	}


	bool ApplicationLayer::OnEvent(const Event& event)
	{
		Application& application = Application::Get();
        return application.OnEvent(event);
	}


	uint32 ApplicationLayer::GetRecivableCategories() const
	{
		return EVENT_CATEGORY_ALL;
	}

	//-----------
	//Application
	//-----------

	Application* Application::s_pInstance = nullptr;

	Application::Application(const EngineParams& params)
		: m_pWindow(nullptr),
		m_pImGuiLayer(nullptr),
		m_pApplicationLayer(nullptr),
		m_Params(params),
		m_ExitCode(0),
		m_Running(true)
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;
	}


	int32 Application::Run()
	{
		InternalOnLoad();

        LOG_DEBUG_INFO("Lambda Engine: STARTING\n");
        
		Clock clock;
		Timestep accumulator;
		const Timestep timestep = Timestep::Seconds(1.0f / 60.0f);
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
            if (m_pWindow->HasFocus())
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
                LOG_SYSTEM_PRINT("FPS: %u, UPS: %u, Frametime: %.1fms\n", fps, ups, clock.GetDeltaTime().AsMilliSeconds());
                fps = 0;
                ups = 0;
                clock.Reset();
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

	
	ImGuiLayer* Application::GetUILayer() const
	{
		return m_pImGuiLayer;
	}


	void Application::Quit(int32 exitCode)
	{
		m_Running = false;
		m_ExitCode = exitCode;
        
        LOG_DEBUG_INFO("Quit called\n");
	}


	Application& Application::Get()
	{
		LAMBDA_ASSERT(s_pInstance != nullptr);
		return *s_pInstance;
	}
	

	void Application::InternalOnLoad()
	{
		//Setup the eventdispatcher
		EventDispatcher::Initialize();


		//Setup application layer
		{
			m_pApplicationLayer = DBG_NEW ApplicationLayer();
			EventDispatcher::PushEventLayer(m_pApplicationLayer);
		}


		//Create window
		{
			WindowDesc desc = {};
			desc.pTitle				= m_Params.pTitle;
			desc.Width				= m_Params.WindowWidth;
			desc.Height				= m_Params.WindowHeight;
			desc.GraphicsDeviceAPI	= m_Params.GraphicsDeviceApi;
			desc.SampleCount		= m_Params.SampleCount;
			desc.Fullscreen			= m_Params.Fullscreen;

			m_pWindow = IWindow::Create(desc);
			m_pWindow->SetEventCallback(EventDispatcher::DispatchEvent);

			//Push Resize callback
			EventDispatcher::PushCallback<IGraphicsDevice, WindowResizeEvent>(IGraphicsDevice::Get(), &IGraphicsDevice::OnResize);
		}


		//Create ImGUI-Layer
		{
			m_pImGuiLayer = DBG_NEW ImGuiLayer();
			//Push ImGui-Layer
			EventDispatcher::PushEventLayer(m_pImGuiLayer);
		}


		//Set joystick-pollingrate
		JoystickManager::SetPollrate(Timestep::Seconds(1.0f / 60.0f));


		//Load rest of application
		OnLoad();
	}
	

	void Application::InternalOnUpdate(Timestep dt)
	{
        //Window update (Handle events)
        m_pWindow->OnUpdate();
        
        //Update controllers
		JoystickManager::OnUpdate();
		
        //Call update 
        OnUpdate(dt);
	}


	void Application::InternalOnRender(Timestep dt)
	{
		OnRender(dt);
	}


	void Application::InternalOnRelease()
	{
		OnRelease();
       
		//Release Eventdispatcher
		EventDispatcher::Release();
		m_pWindow->SetEventCallback(nullptr);

		//Destroy ImGui-Layer
		SafeDelete(m_pImGuiLayer);
        //Destroy applicationlayer
		SafeDelete(m_pApplicationLayer);	
		//Destroy window
        SafeDelete(m_pWindow);
	}


	bool Application::OnEvent(const Event& event)
	{
		switch (event.GetType())
		{
			//Exit the application when the window is closed
		case WindowClosedEvent::GetStaticType():
			Quit(0);
			LOG_DEBUG_INFO("Window closed\n");
			return true;

			//Exit the application when the escape key is pressed
		case KeyPressedEvent::GetStaticType():
			if (static_cast<const KeyPressedEvent&>(event).GetKey() == KEY_ESCAPE)
			{
				Quit(0);
				LOG_DEBUG_INFO("Escape pressed, exiting\n");
				return true;
			}
			else
			{
				return false;
			}

			//If not an handled event return false to let it continue in the eventhandler
		default:
			return false;
		}
	}
}
