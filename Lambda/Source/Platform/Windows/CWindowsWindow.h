#pragma once
#include "LambdaCore.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#if !defined(WIN32_LEAN_AND_MEAN)
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>
	#include <vector>
	#include "../Desktop/CBaseWindow.h"

namespace Lambda
{
	//--------------
	//CWindowsWindow
	//--------------

	class CWindowsWindow final : public CBaseWindow
	{
		friend LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	public:
		CWindowsWindow(const char* pTitle, uint32 width, uint32 height);
		~CWindowsWindow();

		virtual bool HasFocus() const override;
        virtual uint32 GetHeight() const override;
        virtual uint32 GetWidth() const override;

        bool GetFullscreen() const;
        bool SetFullscreen(bool fullscreen);
	private:
		void Init(const char* pTitle, uint32 width, uint32 height);
		LRESULT OnEvent(uint32 msg, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_hWindow;
		uint32 m_Height;
		uint32 m_Width;
		DWORD m_Style;
		DWORD m_ExStyle;
		bool m_Fullscreen;
		bool m_HasFocus;
	};
}
#endif
