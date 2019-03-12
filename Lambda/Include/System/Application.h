#include <LambdaPch.h>
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
		void Quit();
		void InternalOnLoad();
		void InternalOnUpdate(Time dt);
		void InternalOnRelease();

	private:
		IWindow* m_pWindow;
		bool m_Running;

	public:
		static Application& GetInstance();

	private:
		static bool OnEvent(const Event& event);

	private:
		static Application* s_pInstance;
	};
}