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
		virtual void PrintF(const char* pFormat, ...) override final;

		//Window Events
		virtual void OnWindowMove(uint32 x, uint32 y) override final;
		virtual void OnWindowFocusChanges(bool hasFocus) override final;
		virtual void OnWindowResize(uint32 width, uint32 height) override final;
		virtual void OnWindowClose() override final;
	private:
		WindowsWindow* m_pWindow;
	};
}