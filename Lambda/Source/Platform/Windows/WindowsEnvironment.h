#pragma once
#include "Core/Environment.h"
#include "Core/IWindowEventListener.h"

namespace Lambda
{
	class WindowsWindow;

	//------------------
	//WindowsEnvironment
	//------------------

	class WindowsEnvironment final : public Environment, public IWindowEventListener
	{
	public:
		WindowsEnvironment() = default;
		~WindowsEnvironment() = default;

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