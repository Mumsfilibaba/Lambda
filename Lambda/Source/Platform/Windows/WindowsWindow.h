#pragma once
#include "System/IWindow.h"
#if defined(LAMBDA_PLAT_WINDOWS)
namespace Lambda
{
	class WindowsWindow : public IWindow
	{
		friend LRESULT CALLBACK WindowEventCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	public:
		LAMBDA_NO_COPY(WindowsWindow);

		WindowsWindow(const WindowDesc& desc);
		~WindowsWindow();

		virtual void SetEventCallback(EventCallback callback) override;
		virtual void OnUpdate() const override;

        virtual IGraphicsDevice* GetGraphicsDevice() const override final;
		virtual void* GetNativeHandle() const override;
		virtual uint32 GetHeight() const override;
		virtual uint32 GetWidth() const override;

	private:
		void Init(const WindowDesc& desc);
		LRESULT OnEvent(uint32 msg, WPARAM wParam, LPARAM lParam);

	private:
		EventCallback m_OnEvent;
		HWND m_Wnd;
		uint32 m_Height;
		uint32 m_Width;
	};
}
#endif
