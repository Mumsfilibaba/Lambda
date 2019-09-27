#include "LambdaPch.h"
#include "System/Application.h"
#include "Graphics/UILayer.h"
#include "Time/Clock.h"
#include "System/Log.h"
#include "System/Input.h"
#include "System/JoystickManager.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"

namespace Lambda
{
	//-----------
	//Application
	//-----------

	Application* Application::s_pInstance = nullptr;

	Application::Application(const EngineParams& params)
		: m_pWindow(nullptr),
		m_pUILayer(nullptr),
		m_Params(params),
		m_ExitCode(0),
		m_Running(true)
	{
		LAMBDA_ASSERT(s_pInstance == nullptr);
		s_pInstance = this;
	}


    void Application::PushLayer(Layer* pLayer)
    {
        m_LayerStack.PushLayer(pLayer);
    }


	int32 Application::Run()
	{
		OnLoad();

        LOG_DEBUG_INFO("Lambda Engine: STARTING\n");
        
		Clock clock;
		Timestep accumulator;
		const Timestep timestep = Timestep::Seconds(1.0f / 60.0f);
		uint32 ups = 0;

		clock.Reset();
		while (m_Running)
		{
            //Tick
			clock.Tick();

			//Logic update
			accumulator += clock.GetDeltaTime();
			while (accumulator >= timestep)
            {
				OnUpdate(timestep);
				accumulator -= timestep;
                ups++;
			}
            
			//Draw UI
            OnRenderUI(clock.GetDeltaTime());
            
			//Render
#if defined(LAMBDA_PLAT_MACOS)
            if (m_pWindow->HasFocus())
            {
                //Render when the application has focus
                OnRender(clock.GetDeltaTime());
            }
            else
            {
                //If the application does not have focus sleep for one frame (16ms)
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
#else
			OnRender(clock.GetDeltaTime());
#endif

			//Print UPS to console
			if (clock.GetTotalTime().AsSeconds() >= 1.0f)
			{
                LOG_SYSTEM_PRINT("UPS: %u, Frametime: %.1fms\n", ups, clock.GetDeltaTime().AsMilliSeconds());
                ups = 0;
                clock.Reset();
			}
		}

        LOG_DEBUG_INFO("Lambda Engine: EXITING\n");        
		OnRelease();
		return m_ExitCode;
	}


	void Application::OnLoad()
	{
		//Setup the eventdispatcher
        m_Dispatcher.Init();
        //Setup layers
        m_LayerStack.Init();
		
		//Create window
		WindowDesc desc = {};
		desc.pTitle				= m_Params.pTitle;
		desc.Width				= m_Params.WindowWidth;
		desc.Height				= m_Params.WindowHeight;
		desc.GraphicsDeviceAPI	= m_Params.GraphicsDeviceApi;
		desc.SampleCount		= m_Params.SampleCount;
		desc.Fullscreen			= m_Params.Fullscreen;
        desc.VerticalSync       = m_Params.VerticalSync;
		m_pWindow = IWindow::Create(desc);
		m_pWindow->SetEventCallback(DBG_NEW ObjectEventCallback(this, &Application::OnEvent));
		//Push Resize callback
		PushCallback(IGraphicsDevice::Get(), &IGraphicsDevice::OnResize);

		//Create UI-Layer
		m_pUILayer = DBG_NEW UILayer();
		//Push ImGui-Layer
        PushLayer(m_pUILayer);
		
		//Set joystick-pollingrate
		JoystickManager::SetPollrate(Timestep::Seconds(1.0f / 60.0f));

		//Init renderer
		m_Renderer.Init();

		//Load rest of application
        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); it++)
            (*it)->OnLoad();
	}
	

	void Application::OnUpdate(Timestep dt)
	{
        //Window update (Handle events)
        m_pWindow->OnUpdate();
        //Update controllers
		JoystickManager::OnUpdate();
        //Call update
        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); it++)
            (*it)->OnUpdate(dt);
	}


	void Application::OnRender(Timestep dt)
	{
		m_Renderer.Begin();

        for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); it++)
            (*it)->OnRender(dt, m_Renderer.m_pCurrentList);

		m_Renderer.End();
		m_Renderer.Swapbuffers();
	}


    void Application::OnRenderUI(Timestep dt)
    {
		m_pUILayer->Begin(dt);
		for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); it++)
		{
			Layer* pLayer = (*it);
            pLayer->OnRenderUI(dt);
		}
		m_pUILayer->End();
    }


	void Application::OnRelease()
	{
		//Release renderer
		m_Renderer.Release();
		//Remove callback
		m_pWindow->SetEventCallback(nullptr);
		//Release all layers
		for (auto it = m_LayerStack.Begin(); it != m_LayerStack.End(); it++)
		{
			Layer* pLayer = (*it);
			pLayer->OnRelease();
		}
        //Release LayerStack
        m_LayerStack.Release();
        //Release Eventdispatcher
        m_Dispatcher.Release();
		//Destroy window
		SafeDelete(m_pWindow);
	}


	bool Application::OnEvent(const Event& event)
	{
        //Forward to the rest of application
        EventForwarder forwarder;
        forwarder.ForwardEvent(this, &Application::OnWindowClose, event);
        forwarder.ForwardEvent(this, &Application::OnKeyPressed, event);
		forwarder.ForwardEvent(this, &Application::OnWindowResize, event);

        //Dispatch to all layers
        for (auto it = m_LayerStack.End(); it != m_LayerStack.Begin(); )
        {
            Layer* pLayer = (*--it);
            if (event.GetCategoryFlags() & pLayer->GetRecivableCategories())
            {
                if(pLayer->OnEvent(event))
                {
                    event.SetIsHandled(true);
                    break;
                }
            }
        }
        //Dispatch to all callbacks
        m_Dispatcher.DispatchEvent(event);
        return true;
	}


    bool Application::OnWindowClose(const WindowClosedEvent& event)
    {
        Quit(0);
        LOG_DEBUG_INFO("Window closed\n");
        return true;
    }


    bool Application::OnKeyPressed(const KeyPressedEvent &event)
    {
        if (event.GetKey() == KEY_ESCAPE)
        {
            Quit(0);
            LOG_DEBUG_INFO("Escape pressed, exiting\n");
            return true;
        }
        else
        {
            return false;
        }
    }

	
	bool Application::OnWindowResize(const WindowResizeEvent& event)
	{
		m_Renderer.SetDisplaySize(event.GetWidth(), event.GetHeight());
		return false;
	}


    const EngineParams& Application::GetEngineParams() const
    {
        return m_Params;
    }


    IWindow* Application::GetWindow() const
    {
        return m_pWindow;
    }


    UILayer* Application::GetUILayer() const
    {
        return m_pUILayer;
    }

	
	const Renderer3D& Application::GetRenderer() const
	{
		return m_Renderer;
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
}
