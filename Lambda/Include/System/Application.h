#pragma once
#include "IWindow.h"
#include "Time/Clock.h"
#include "Events/EventDispatcher.h"
#include "Graphics/IGraphicsDevice.h"
#include "Graphics/ImGuiLayer.h"

namespace Lambda
{
	//------------
	//EngineParams
	//------------

	struct EngineParams
	{
		const char* pTitle = nullptr;
		GraphicsApi GraphicsDeviceApi = GRAPHICS_API_VULKAN;
		uint32 WindowWidth = 1920;
		uint32 WindowHeight = 1080;
		uint32 SampleCount = 1;
		bool Fullscreen = false;
	};

	//----------------
	//ApplicationLayer
	//----------------

	class ApplicationLayer : public EventLayer
	{
	public:
		LAMBDA_NO_COPY(ApplicationLayer);

		ApplicationLayer();
		~ApplicationLayer() = default;

		virtual void 	OnPush() override final;
		virtual void 	OnPop() override final;
		virtual bool 	OnEvent(const Event& event) override final;
		virtual uint32	GetRecivableCategories() const override final;
	};

	//-----------
	//Application
	//-----------

	class LAMBDA_API Application
	{
	public:
		Application(const EngineParams& params);
		virtual ~Application() = default;

		virtual void OnLoad() {}
		virtual void OnUpdate(Timestep dt) {}
		virtual void OnRender(Timestep dt) {}
		virtual void OnRelease() {}
				
		bool OnEvent(const Event& event);

		int32 Run();

		const EngineParams& GetEngineParams() const;
		IWindow* 			GetWindow() const;

	private:
		void Init(const EngineParams& params);
		void Quit(int32 exitCode = 0);
		void InternalOnLoad();
		void InternalOnUpdate(Timestep dt);
		void InternalOnRender(Timestep dt);
		void InternalOnRelease();

	private:
		IWindow* 			m_pWindow;
		ImGuiLayer*	 		m_pImGuiLayer;
		ApplicationLayer* 	m_pApplicationLayer;
        GraphicsLayer*      m_pGraphicsLayer;
		EngineParams 		m_Params;
		int32 				m_ExitCode;
		bool 				m_Running;
		bool 				m_HasFocus;

	public:
		static Application& Get();

	private:
		static Application* s_pInstance;
	};
}
