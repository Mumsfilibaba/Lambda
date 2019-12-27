#pragma once
#include "LambdaCore.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <vector>
	#include "../Desktop/IWindow.h"

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	//--------------
	//CWindowsWindow
	//--------------
	class CWindowsWindow final : public IWindow
	{
	public:
		CWindowsWindow(const char* pTitle, uint32 width, uint32 height);
		~CWindowsWindow();

		virtual bool HasFocus() const override;
        virtual uint32 GetHeight() const override;
        virtual uint32 GetWidth() const override;

        bool GetFullscreen() const;
        bool SetFullscreen(bool fullscreen);

		LRESULT OnEvent(uint32 msg, WPARAM wParam, LPARAM lParam);

		_forceinline HWND GetHWND() const
		{
			return m_hWindow;
		}
	private:
		void Init(const char* pTitle, uint32 width, uint32 height);
	private:
		HWND m_hWindow;
		uint32 m_Height;
		uint32 m_Width;
		uint32 m_PosX;
		uint32 m_PosY;
		DWORD m_Style;
		DWORD m_ExStyle;
		bool m_Fullscreen;
		bool m_HasFocus;
		bool m_IsAlive;
	};
}
#endif
