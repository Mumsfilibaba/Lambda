#pragma once
#include "Core/IWindow.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include "Windows.h"
	
	#include <vector>

namespace Lambda
{
	struct SEvent;

	class LAMBDA_API CWindowsWindow : public IWindow
	{
	public:
		CWindowsWindow(const SWindowDesc& desc);
		~CWindowsWindow();

		virtual void Show() override;

		virtual void AddEventListener(IEventListener* pEventListener) override;
		virtual void RemoveEventListener(IEventListener* pEventListener) override;

		virtual uint32 GetWidth() const override { return m_Width; }
		virtual uint32 GetHeight() const override { return m_Height; }
		virtual void* GetNativeHandle() const override { return (void*)m_hWindow; }

	public:
		HWND GetHandle() const { return m_hWindow; }
		LRESULT ProcessMessage(uint32 message, WPARAM wParam, LPARAM lParam);

		static void RegisterWindowClass(HINSTANCE hInstance);
		static void UnregisterWindowClass(HINSTANCE hInstance);
#ifdef UNICODE
		static LPCWSTR GetWindowClass() { return L"AppWindow"; }
#else
		static LPCSTR GetWindowClass() { return "AppWindow"; }
#endif

	private:
		void Init(const SWindowDesc& desc);
		void DispatchEvent(const SEvent& event);

	private:
		std::vector<IEventListener*> m_EventListeners;
		HWND m_hWindow;
		uint32 m_Width;
		uint32 m_Height;
		uint32 m_Style;
		uint32 m_ExStyle;
		bool m_bIsMiniaturized;
	};
}
#endif