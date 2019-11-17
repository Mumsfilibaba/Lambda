#include "LambdaPch.h"
#include "Core/Application.h"
#include "Debug/DebugLayer.h"
#include "Graphics/Core/IFactory.h"
#include "Time/Clock.h"
#include "Core/LogManager.h"
#include "Core/Input.h"
#include "Core/GamePadManager.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvent.h"

namespace Lambda
{
	//-----------
	//Application
	//-----------

	Application::Application(const EngineParams& params)
		: Singleton<Application>(),
		m_pWindow(nullptr),
		m_pGamePadManager(nullptr),
		m_pUILayer(nullptr),
        m_GraphicsDevice(nullptr),
        m_Context(nullptr),
        m_SwapChain(nullptr),
		m_Params(params),
		m_ExitCode(0),
		m_Running(true)
	{
	}


    void Application::PushLayer(Layer* pLayer)
    {
        m_LayerStack.PushLayer(pLayer);
    }


	int32 Application::Run()
	{
		OnLoad();

        LOG_DEBUG_INFO("[LAMBDA ENGINE] STARTING\n");
        
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
                LOG_SYSTEM_PRINT("[LAMBDA ENGINE] UPS: %u, Frametime: %.1fms\n", ups, clock.GetDeltaTime().AsMilliSeconds());
                ups = 0;
                clock.Reset();
			}
		}

        LOG_DEBUG_INFO("[LAMBDA ENGINE] EXITING\n");        
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
		desc.pTitle	= m_Params.pTitle;
		desc.Width	= m_Params.WindowWidth;
		desc.Height	= m_Params.WindowHeight;
		m_pWindow = IWindow::Create(desc);
		m_pWindow->SetEventCallback(DBG_NEW ObjectEventCallback(this, &Application::OnEvent));
		
		//Init GamePad-Manager
		m_pGamePadManager = GamePadManager::Create();
		GamePadManager::SetPollrate(Timestep::Seconds(1.0f / 60.0f));

        //Create factor
        AutoRef<IFactory> factory = nullptr;
        IFactory::CreateFactory(&factory, m_Params.GraphicsDeviceApi);
		//Setup device
        DeviceDesc deviceDesc = {};
#if defined(LAMBDA_DEBUG)
        deviceDesc.Flags = DEVICE_FLAG_DEBUG;
#else
        deviceDesc.Flags = DEVICE_FLAG_NONE;
#endif
        //Setup swapchain
        SwapChainDesc swapChainDesc = {};
        swapChainDesc.pWindowHandle     = m_pWindow->GetNativeHandle();
        swapChainDesc.BufferCount       = 3;
        swapChainDesc.BufferFormat      = FORMAT_B8G8R8A8_UNORM;
        swapChainDesc.DepthBufferFormat = FORMAT_D24_UNORM_S8_UINT;
        swapChainDesc.BufferHeight      = 0;
        swapChainDesc.BufferWidth       = 0;
        swapChainDesc.BufferSampleCount = m_Params.SampleCount;
        swapChainDesc.VerticalSync      = m_Params.VerticalSync;
        factory->CreateDeviceAndSwapChain(&m_GraphicsDevice, deviceDesc, &m_Context, &m_SwapChain, swapChainDesc);
    
		//Init Debug-Layer
		m_pUILayer = DBG_NEW DebugLayer();
        PushLayer(m_pUILayer);

		//Init renderer
		m_Renderer.Init();

		//Load rest of application
		for (auto pLayer : m_LayerStack)
			pLayer->OnLoad();
	}
	

	void Application::OnUpdate(Timestep dt)
	{
        //Window update (Handle events)
        m_pWindow->OnUpdate();
        //Update controllers
		GamePadManager::OnUpdate();
        //Call update
        for (auto pLayer : m_LayerStack)
            pLayer->OnUpdate(dt);
	}


	void Application::OnRender(Timestep dt)
	{
		m_Renderer.Begin();
        for (auto pLayer : m_LayerStack)
            pLayer->OnRender(m_Renderer, dt);
		m_Renderer.End();
		m_Renderer.Swapbuffers();
	}


    void Application::OnRenderUI(Timestep dt)
    {
		m_pUILayer->Begin(dt);
        for (auto pLayer : m_LayerStack)
            pLayer->OnRenderUI(dt);
		m_pUILayer->End();
    }


	void Application::OnRelease()
	{
		//Release renderer
		m_Renderer.Release();
        
		//Remove callback
		m_pWindow->SetEventCallback(nullptr);
		
		//Release all layers
		for (auto pLayer : m_LayerStack)
			pLayer->OnRelease();
        
		//Release LayerStack
        m_LayerStack.Release();
        
		//Release Eventdispatcher
        m_Dispatcher.Release();

        //Destroy graphics objects
        m_SwapChain.Release();
        m_Context.Release();
        m_GraphicsDevice.Release();
        
		//Destroy GamePadManager
		SafeDelete(m_pGamePadManager);
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
        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
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
        LOG_DEBUG_INFO("[LAMBDA ENGINE] Window closed\n");
        return true;
    }


    bool Application::OnKeyPressed(const KeyPressedEvent &event)
    {
        if (event.GetKey() == KEY_ESCAPE)
        {
            Quit(0);
            LOG_DEBUG_INFO("[LAMBDA ENGINE] Escape pressed, exiting\n");
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


    DebugLayer* Application::GetUILayer() const
    {
        return m_pUILayer;
    }


    IDevice* Application::GetGraphicsDevice() const
    {
        return m_GraphicsDevice.Get();
    }


    ISwapChain* Application::GetSwapChain() const
    {
        return m_SwapChain.Get();
    }


	Renderer3D& Application::GetRenderer()
	{
		return m_Renderer;
	}

	
	const Renderer3D& Application::GetRenderer() const
	{
		return m_Renderer;
	}


    void Application::Quit(int32 exitCode)
    {
        m_Running = false;
        m_ExitCode = exitCode;
        
        LOG_DEBUG_INFO("[LAMBDA ENGINE] Quit called\n");
    }
}
