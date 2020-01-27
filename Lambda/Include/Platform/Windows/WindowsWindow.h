#pragma once
#include "Platform/Generic/GenericWindow.h"

#ifdef LAMBDA_PLAT_WINDOWS

namespace Lambda
{
	class CWindowsWindow : public CGenericWindow
	{
	public:
		CWindowsWindow(const SWindowDesc& desc);
		~CWindowsWindow();

		virtual void Show() override;

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
	private:
		HWND m_hWindow;
		uint32 m_Width;
		uint32 m_Height;
		uint32 m_Style;
		uint32 m_ExStyle;
	};
}
#endif