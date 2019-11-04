#pragma once
#include "IWindow.h"
#include "LayerStack.h"
#include "GamePadManager.h"
#include "Time/Clock.h"
#include "Events/EventDispatcher.h"
#include "Debug/DebugLayer.h"
#include "Graphics/Core/IDevice.h"
#include "Graphics/Core/ISwapChain.h"
#include "Graphics/Renderer3D.h"

namespace Lambda
{
	//------------
	//EngineParams
	//------------

	struct EngineParams
	{
		const char* pTitle              = nullptr;
		GraphicsAPI GraphicsDeviceApi   = GRAPHICS_API_VULKAN;
		uint32 WindowWidth              = 1920;
		uint32 WindowHeight             = 1080;
		uint32 SampleCount              = 1;
		bool Fullscreen                 = false;
        bool VerticalSync               = true;
	};

	//-----------
	//Application
	//-----------

	class LAMBDA_API Application : public Singleton<Application>
	{
	public:
		Application(const EngineParams& params);
		virtual ~Application() = default;

		void Quit(int32 exitCode = 0);
        int32 Run();

        IWindow* GetWindow() const;
        DebugLayer* GetUILayer() const;
        IDevice* GetGraphicsDevice() const;
        ISwapChain* GetSwapChain() const;
		Renderer3D& GetRenderer();
		const Renderer3D& GetRenderer() const;
        const EngineParams& GetEngineParams() const;


		//Push a layer onto the layerstack
        void PushLayer(Layer* pLayer);
        //Push a global callback function
        template <typename EventT>
		inline void PushCallback(bool(*callbackFunc)(const EventT&))
        {
            m_Dispatcher.PushCallback(EventT::GetStaticType(), DBG_NEW EventCallback<EventT>(callbackFunc));
        }


        //Push object callback function
        template <typename ObjectType, typename EventT>
        inline void PushCallback(ObjectType* pObjectRef, bool(ObjectType::* objectCallbackFunc)(const EventT&))
        {
            m_Dispatcher.PushCallback(EventT::GetStaticType(), DBG_NEW ObjectEventCallback<ObjectType, EventT>(pObjectRef, objectCallbackFunc));
        }
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
	private:
		IWindow* m_pWindow;
		DebugLayer*	m_pUILayer;
		GamePadManager* m_pGamePadManager;
        AutoRef<IDevice> m_GraphicsDevice;
        AutoRef<IDeviceContext> m_Context;
        AutoRef<ISwapChain> m_SwapChain;
        EventDispatcher m_Dispatcher;
        LayerStack m_LayerStack;
		Renderer3D m_Renderer;
		EngineParams m_Params;
		int32 m_ExitCode;
		bool m_Running;
	};
}
