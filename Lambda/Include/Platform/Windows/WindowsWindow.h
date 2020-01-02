#pragma once
#include "Core/Engine/IWindow.h"

#if defined(LAMBDA_PLAT_WINDOWS)	
	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	class CWindowsSystem;

	//--------------
	//CWindowsWindow
	//--------------
	class CWindowsWindow : public IWindow
	{
	public:
		CWindowsWindow(CWindowsSystem* pSystem);
		~CWindowsWindow();

		/*IWindow Interface*/
		virtual bool HasFocus() const override final { return m_bHasFocus; }
		
		virtual uint32	GetWidth()  const override final { return m_Width; }
		virtual uint32	GetHeight() const override final { return m_Height; }
		virtual void*	GetNativeHandle() const override final { return (void*)m_hWindow; }
		
		/*CWindowsWindow*/
		bool Initialize(const char* pTitle, uint32 width, uint32 height);

		LRESULT OnMessage(uint32 message, WPARAM wParam, LPARAM lParam);
	private:
		CWindowsSystem* m_pSystem;
		HWND m_hWindow;
		uint32 m_Width;
		uint32 m_Height;
		uint32 m_Style;
		uint32 m_ExStyle;
		struct
		{
			uint32 x;
			uint32 y;
		} m_Position;
		bool m_bHasFocus;
	};
}
#endif