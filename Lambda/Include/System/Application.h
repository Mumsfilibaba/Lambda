#pragma once
#include "IWindow.h"
#include "LayerStack.h"
#include "Time/Clock.h"
#include "Events/EventDispatcher.h"
#include "Graphics/IGraphicsDevice.h"
#include "Graphics/UILayer.h"
#include "Graphics/Renderer3D.h"
#include "Debug/DebugLayer.h"

namespace Lambda
{
	//------------
	//EngineParams
	//------------

	struct EngineParams
	{
		const char* pTitle              = nullptr;
		GraphicsApi GraphicsDeviceApi   = GRAPHICS_API_VULKAN;
		uint32 WindowWidth              = 1920;
		uint32 WindowHeight             = 1080;
		uint32 SampleCount              = 1;
		bool Fullscreen                 = false;
        bool VerticalSync               = true;
	};

	//-----------
	//Application
	//-----------

	class LAMBDA_API Application
	{
	public:
		Application(const EngineParams& params);
		virtual ~Application() = default;

        int32 Run();

        //Push a layer onto the layerstack
        void PushLayer(Layer* pLayer);
        //Push a global callback function
        template <typename EventT>
        void PushCallback(bool(*callbackFunc)(const EventT&))
        {
            m_Dispatcher.PushCallback(EventT::GetStaticType(), DBG_NEW EventCallback<EventT>(callbackFunc));
        }
        //Push object callback function
        template <typename ObjectType, typename EventT>
        void PushCallback(ObjectType* pObjectRef, bool(ObjectType::* objectCallbackFunc)(const EventT&))
        {
            m_Dispatcher.PushCallback(EventT::GetStaticType(), DBG_NEW ObjectEventCallback<ObjectType, EventT>(pObjectRef, objectCallbackFunc));
        }
        
        IWindow* GetWindow() const;
        UILayer* GetUILayer() const;
		const Renderer3D& GetRenderer() const;
        const EngineParams& GetEngineParams() const;
    private:
        void OnLoad();
        void OnUpdate(Timestep dt);
        void OnRender(Timestep dt);
        void OnRenderUI(Timestep dt);
        void OnRelease();
		bool OnEvent(const Event& event);
        bool OnWindowClose(const WindowClosedEvent& event);
        bool OnKeyPressed(const KeyPressedEvent& event);
		bool OnWindowResize(const WindowResizeEvent& event);
        
		void Quit(int32 exitCode = 0);
	private:
		IWindow* 			m_pWindow;
		UILayer*	 		m_pUILayer;
        EventDispatcher     m_Dispatcher;
        LayerStack          m_LayerStack;
		Renderer3D			m_Renderer;
		EngineParams 		m_Params;
		int32 				m_ExitCode;
		bool 				m_Running;
	public:
		static Application& Get();
	private:
		static Application* s_pInstance;
	};
}
