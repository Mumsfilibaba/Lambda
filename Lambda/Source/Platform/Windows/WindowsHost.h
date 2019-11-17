#pragma once
#include "Core/Host.h"
#include "Core/IWindowEventListener.h"

namespace Lambda
{
	class WindowsWindow;

	//-----------
	//WindowsHost
	//-----------

	class WindowsHost final : public Host, public IWindowEventListener
	{
	public:
		WindowsHost() = default;
		~WindowsHost() = default;

		virtual void Init() override final;
		virtual void ProcessEvents() override final;
		virtual void Release() override final;

		//Window Events
		virtual void OnWindowResize(uint32 width, uint32 height) override final;
		virtual void OnWindowDestroy() override final;
	private:
		WindowsWindow* m_pWindow;
	};
}