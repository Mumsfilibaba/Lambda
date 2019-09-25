#pragma once
#include "System/IWindow.h"
#if defined(LAMBDA_PLAT_WINDOWS)
	#include <vector>

namespace Lambda
{
	//-------------
	//WindowsWindow
	//-------------

	class WindowsWindow : public IWindow
	{
		friend LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		LAMBDA_NO_COPY(WindowsWindow);

		WindowsWindow(const WindowDesc& desc);
		~WindowsWindow();

		virtual void OnUpdate() const override final;
		virtual bool HasFocus() const override final;
		virtual void SetEventCallback(IEventCallback* pCallback) override final;
        virtual bool SetFullscreen(bool fullscreen) override final;
        virtual bool GetFullscreen() const override final;
        virtual IGraphicsDevice* GetGraphicsDevice() const override final;
        virtual void* GetNativeHandle() const override;
        virtual uint32 GetHeight() const override;
        virtual uint32 GetWidth() const override;
	private:
		void Init(const WindowDesc& desc);
		void DispatchEvent(const Event& event);
		LRESULT OnEvent(uint32 msg, WPARAM wParam, LPARAM lParam);
	private:
		IGraphicsDevice*	m_pGraphicsDevice;
		IEventCallback*		m_pEventCallback;
		HWND				m_hWindow;
		uint32				m_Height;
		uint32				m_Width;
		DWORD				m_Style;
		DWORD				m_ExStyle;
		bool				m_Fullscreen;
		bool				m_HasFocus;
	};
}
#endif
