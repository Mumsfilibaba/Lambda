#pragma once
#include "IWindow.h"
#include "Clock.h"
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

	//-----------
	//Application
	//-----------

	class LAMBDA_API Application
	{
	public:
		Application(const EngineParams& params);
		virtual ~Application() = default;

		virtual void OnLoad() {}
		virtual void OnUpdate(Time dt) {}
		virtual void OnRender(Time dt) {}
		virtual void OnRelease() {}

		int32 Run();
		const EngineParams& GetEngineParams() const;
		IWindow* GetWindow() const;

	private:
		void Init(const EngineParams& params);
		void Quit(int32 exitCode = 0);
		void InternalOnLoad();
		void InternalOnUpdate(Time dt);
		void InternalOnRender(Time dt);
		void InternalOnRelease();
		bool InternalOnEvent(const Event& event);

	private:
		IWindow* m_pWindow;
		ImGuiLayer* m_pImGuiLayer;
		EngineParams m_Params;
		int32 m_ExitCode;
		bool m_Running;
        bool m_HasFocus;

	public:
		static Application& GetInstance();

	private:
		static bool OnEvent(const Event& event);

	private:
		static Application* s_pInstance;
	};
}
