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


	bool ApplicationLayer::OnEvent(const Event* pEvent)
	{
		Application& application = Application::Get();
        return application.OnEvent(pEvent);
	}


	uint32 ApplicationLayer::GetRecivableCategories() const
	{
		return EVENT_CATEGORY_WINDOW | EVENT_CATEGORY_KEYBOARD;
	}

	//-----------
	//Application
	//-----------

	Application* Application::s_pInstance = nullptr;

	Application::Application(const EngineParams& params)
		: m_pWindow(nullptr),
		m_pImGuiLayer(nullptr),
		m_pApplicationLayer(nullptr),
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
			m_pApplicationLayer = DBG_NEW ApplicationLayer();
			EventDispatcher::PushEventLayer(m_pApplicationLayer);
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
			m_pWindow->SetEventCallback(EventDispatcher::DispatchEvent);

			//Push graphics layer
            m_pGraphicsLayer = DBG_NEW GraphicsLayer();
            EventDispatcher::PushEventLayer(m_pGraphicsLayer);
		}

		//Create ImGUI-Layer
		{
			m_pImGuiLayer = DBG_NEW ImGuiLayer();
			//Push ImGui-Layer
			EventDispatcher::PushEventLayer(m_pImGuiLayer);
		}

		//Set joystick-pollingrate
		JoystickManager::SetPollrate(Timestep::Seconds(1.0f / 60.0f));

		//Set Params
		m_Params = params;
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
       
		//Destroy ImGui-Layer
		SafeDelete(m_pImGuiLayer);
        //Destroy applicationlayer
		SafeDelete(m_pApplicationLayer);
		//Destroy window
        SafeDelete(m_pWindow);
	}


	bool Application::OnEvent(const Event* pEvent)
	{
		switch (pEvent->GetType())
		{
			//Exit the application when the window is closed
		case EVENT_TYPE_WINDOW_CLOSED:
			Quit(0);
			LOG_DEBUG_INFO("Window closed\n");
			return true;

			//Exit the application when the escape key is pressed
		case EVENT_TYPE_KEY_PRESSED:
			if (static_cast<const KeyPressedEvent*>(pEvent)->GetKey() == KEY_ESCAPE)
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
		case EVENT_TYPE_WINDOW_FOCUS_CHANGED:
			if (IGraphicsDevice::Get())
			{
				IGraphicsDevice::Get()->WaitForGPU();
			}

			m_HasFocus = static_cast<const WindowFocusChangedEvent*>(pEvent)->HasFocus();

			//Return false so that other parts of the app still can get this event
			return false;

			//If not an handled event return false to let it continue in the eventhandler
		default:
			return false;
		}
	}
}
