#pragma once
#include "Platform/Common/CommonWindow.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#include <vector>

namespace Lambda
{
	struct SEvent;

	class LAMBDA_API CWindowsWindow : public CCommonWindow
	{
	public:
		CWindowsWindow(const SWindowDesc& desc);
		~CWindowsWindow();

		virtual void Show() override;

		virtual void AddEventListener(IEventHandler* pEventListener) override;
		virtual void RemoveEventListener(IEventHandler* pEventListener) override;

		virtual uint32 GetHeight() const override	{ return 0; }
		virtual uint32 GetWidth() const override	{ return 0; }

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
		std::vector<IEventHandler*> m_EventListeners;
		HWND m_hWindow;
		uint32 m_Width;
		uint32 m_Height;
		uint32 m_Style;
		uint32 m_ExStyle;
		bool m_bIsMiniaturized;
	};
}
#endif