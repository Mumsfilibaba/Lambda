#pragma once
#include "IWindow.h"
#include "Time.hpp"

namespace Lambda
{
	class LAMBDA_API Application
	{
	public:
		Application();
		~Application();

		virtual void OnLoad() {}
		virtual void OnUpdate(Time dt) {}
		virtual void OnRelease() {}

		int32 Run();

	private:
		void Init();
		void Quit(int32 exitCode = 0);
		void InternalOnLoad();
		void InternalOnUpdate(Time dt);
		void InternalOnRelease();

	private:
		IWindow* m_pWindow;
		int32 m_ExitCode;
		bool m_Running;

	public:
		static Application& GetInstance();

	private:
		static bool OnEvent(const Event& event);

	private:
		static Application* s_pInstance;
	};
}