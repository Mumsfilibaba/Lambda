#pragma once
#include "Platform/Window.h"

#if defined(LAMBDA_PLAT_WINDOWS)	
	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

	#if defined(IsMaximized)
		#undef IsMaximized
	#endif

namespace Lambda
{
	class CWindowsApplication;

	//--------------
	//CWindowsWindow
	//--------------

	class CWindowsWindow : public CWindow
	{
	public:
		CWindowsWindow(CWindowsApplication* pApplication);
		~CWindowsWindow();

		bool Init(const SWindowProps& props);
		LRESULT ProcessMessage(uint32 message, WPARAM wParam, LPARAM lParam);

		HWND GetHandle() const { return m_hWindow; }

		static LPCWSTR WindowClass() { return L"AppWindow"; }

		/*CWindow Interface*/
		virtual void SetDisplayMode(EDisplayMode displayMode) override final;
		virtual EDisplayMode GetDisplayMode() const override final { return m_DisplayMode; }

		virtual void Show() override final;
		virtual void Hide() override final;
		virtual void Maximize() override final;
		virtual void Minimize() override final;

		virtual bool HasFocus() const override final		{ return m_bHasFocus; }
		virtual bool IsFullscreen() const override final	{ return m_bIsFullscreen; }
		virtual bool IsMiniaturized() const override final	{ return m_bIsMiniaturized; };
		virtual bool IsMaximized() const override final		{ return false; }

		virtual void SetFocus() override final;
		virtual void SetSize(uint32 width, uint32 height) override final;
		virtual void SetPosition(uint32 x, uint32 y) override final;

		virtual uint32 GetWidth()  const override final { return m_Width; }
		virtual uint32 GetHeight() const override final { return m_Height; }
		virtual void* GetNativeHandle() const override final { return (void*)m_hWindow; }

		virtual void GetPosition(uint32& x, uint32& y) const override final
		{
			x = m_Position.x;
			y = m_Position.y;
		}
	private:
		CWindowsApplication* m_pApplication;
		HWND m_hWindow;
		
		EDisplayMode m_DisplayMode;
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
		bool m_bIsFullscreen;
		bool m_bIsMiniaturized;
	};

	//---------------
	//CPlatformWindow
	//---------------

	typedef CWindowsWindow CPlatformWindow;
}
#endif