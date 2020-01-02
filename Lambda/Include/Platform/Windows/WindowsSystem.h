#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Core/Engine/System.h"
	#include "Core/Event/SystemEventDispatcher.h"

	#include "Platform/Windows/WindowsWindow.h"

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	class IEngine;

	//--------------
	//CWindowsSystem
	//--------------

	class CWindowsSystem final : public ISystem
	{
	public:
		CWindowsSystem(IEngine* pEngine);
		~CWindowsSystem();

		/*ISystem Interface*/
		virtual bool Initialize() override final;
		
		virtual bool CreateWindow(const char* pTitle, uint32 width, uint32 height) override final;

		virtual void ProcessSystemEvents() override final;		
		virtual void AddEventListener(ISystemEventListener* pListener) override final;
		virtual void RemoveEventListener(ISystemEventListener* pListener) override final;

		virtual void Release() override final;

		virtual const IWindow* GetWindow() const override final { return m_pWindow; }	

		/*CWindowsSystem*/
		HINSTANCE GetHINSTANCE() const { return m_hInstance; }

		void OnSystemEvent(const SSystemEvent& event);
		LRESULT OnMessage(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam);
	private:
		IEngine* m_pEngine;
		CWindowsWindow* m_pWindow;
		CSystemEventDispatcher* m_pEventDispatcher;

		HINSTANCE m_hInstance;
	public:
		static void InitializeKeyLookupTable();
		static EKey ConvertFromVirtualKey(uint32 keycode);
		static uint32 ConvertToVirtualKey(EKey keycode);

		static LRESULT CALLBACK MessageProc(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam);
	private:
		static EKey	s_WindowsKeyTable[256];
		static uint32 s_KeyTable[uint32(EKey::KEY_LAST) + 1];
	};
}
#endif