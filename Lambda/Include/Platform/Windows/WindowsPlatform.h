#pragma once
#include "Platform/Common/CommonPlatform.h"

#ifdef LAMBDA_PLAT_WINDOWS
	#ifndef WIN32_LEAN_AND_MEAN 
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <Windows.h>

	#ifdef CreateWindow
		#undef CreateWindow
	#endif

namespace Lambda
{
	class LAMBDA_API CWindowsPlatform final : public CCommonPlatform
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CWindowsPlatform);

		static IConsoleOutput* CreateConsoleOutput();
		static CCommonWindow* CreateWindow(const SWindowDesc& desc);

		static void Init(HINSTANCE hInstance);
		static void Release();

		static void PollEvents();

		static void MessageBox(const char* pCaption, const char* pText, uint32 type);
		static void DebuggerOutput(const char* pMessage);

		static LRESULT CALLBACK WndProc(HWND hWnd, uint32 message, WPARAM wParam, LPARAM lParam);
		static HINSTANCE GetApplicationInstance() { return s_hInstance; }
	private:
		static HINSTANCE s_hInstance;
	};

	typedef CWindowsPlatform Platform;
}
#endif