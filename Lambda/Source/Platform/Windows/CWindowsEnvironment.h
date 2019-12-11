#pragma once
#include "Core/CEnvironment.h"

namespace Lambda
{
	class CWindowsWindow;
	class CWindowMoveEvent;
	class CWindowClosedEvent;
	class CWindowResizeEvent;
	class CWindowFocusChangedEvent;

	//------------------
	//WindowsEnvironment
	//------------------

	class CWindowsEnvironment final : public CEnvironment
	{
	public:
		CWindowsEnvironment() = default;
		~CWindowsEnvironment() = default;

		virtual void Init() override final;
		virtual void ProcessEvents() override final;
		virtual void Release() override final;
		virtual void PrintF(const char* pFormat, ...) override final;
	private:
		virtual bool OnEvent(const CEvent& event) override final;
		bool OnWindowClose(const CWindowClosedEvent&);
		bool OnWindowMove(const CWindowMoveEvent& event);
		bool OnWindowResize(const CWindowResizeEvent& event);
		bool OnWindowFocusChanges(const CWindowFocusChangedEvent& event);
	private:
		CWindowsWindow* m_pWindow;
	};
}