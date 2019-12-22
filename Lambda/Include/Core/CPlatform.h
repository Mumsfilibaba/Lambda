#pragma once
#if defined(LAMBDA_PLAT_WINDOWS)
	#include "../Source/Platform/Windows/CWindowsPlatform.h"
#else
	#include "Defines.h"
	#include "Types.h"
#endif

namespace Lambda
{
	//---------
	//CPlatform
	//---------

	class CPlatform
	{
	public:
		LAMBDA_STATIC_CLASS(CPlatform);

		static bool Initialize()
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::Initialize();
#endif
		}


		static void ProcessEvents()
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::ProcessEvents();
#endif
		}


		static void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type)
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::MessageBox(pCaption, pText, type);
#endif
		}


		static bool InitializeConsole()
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			return CWindowsPlatform::InitializeConsole();
#endif
		}


		static void SetConsoleTextColor(EConsoleColor color)
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::SetConsoleTextColor(color);
#endif
		}


		static void ConsolePrint(const char* pFormat, va_list args)
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::ConsolePrint(pFormat, args);
#endif
		}


		static void ReleaseConsole()
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::ReleaseConsole();
#endif
		}


		static void ShowCursor(bool bVisible)
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::ShowCursor(bVisible);
#endif
		}


		static void SetCursorPosition(int32 x, int32 y)
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::SetCursorPosition(x, y);
#endif
		}


		static void GetCursorPosition(int32& outX, int32& outY)
		{
#if defined(LAMBDA_PLAT_WINDOWS)
			CWindowsPlatform::GetCursorPosition(outX, outY);
#endif
		}
	};
}