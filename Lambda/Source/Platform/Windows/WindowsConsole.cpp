#include "LambdaPch.h"
#include ""
#if defined(LAMBDA_PLAT_WINDOWS)
#include "Platform/Windows/WindowsPlatform.h"
#include "Platform/Windows/WindowsConsole.h"
	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	//---------------
	//SWindowsConsole
	//---------------
	struct SWindowsConsole
	{
		bool bHasConsole = false;
	} g_WindowsConsole;

    //---------------
    //NWindowsConsole
    //---------------
    namespace NWindowsConsole
    {
        bool Initialize()
		{
			if (g_WindowsConsole.bHasConsole)
			{
				return true;
			}

			BOOL bResult = ::AllocConsole();
			if (bResult)
			{
				//Attach the console and redirect the standard output to the new console
				::AttachConsole(GetCurrentProcessId());
				freopen("CONIN$", "r", stdin);
				freopen("CONOUT$", "w", stderr);
				freopen("CONOUT$", "w", stdout);

				//Set title of console
				if (!::SetConsoleTitleA("Lambda Engine Debug Output"))
				{
					printf("SetConsoleTitle failed (%d)\n", ::GetLastError());
					return false;
				}

				//TODO: A message should probably be sent to the main application so that it can exit

				g_WindowsConsole.bHasConsole = true;
				return true;
			}
			else
			{
				NWindowsPlatform::MessageBox("AllocConsole failed", "FATAL ERROR", EMessageBoxType::MESSAGE_BOX_TYPE_ERROR);
				return false;
			}
		}


		bool HasConsole()
		{
			return g_WindowsConsole.bHasConsole;
		}


        void Print(const char* pFormat, va_list args)
		{
			constexpr uint32 MAX_BUFFER_COUNT = 1024;
			static char s_Buffer[MAX_BUFFER_COUNT];

			vsnprintf(s_Buffer, MAX_BUFFER_COUNT, pFormat, args);

			printf(s_Buffer);
			::OutputDebugStringA(s_Buffer);
		}


        void Release()
		{
			if (g_WindowsConsole.bHasConsole)
			{
				::FreeConsole();
				g_WindowsConsole.bHasConsole = false;
			}
		}


        void SetTextColor(EConsoleColor color)
		{
			WORD nColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			if (color == EConsoleColor::CONSOLE_COLOR_RED)
			{
				nColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
			}
			else if (color == EConsoleColor::CONSOLE_COLOR_GREEN)
			{
				nColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}
			else if (color == EConsoleColor::CONSOLE_COLOR_YELLOW)
			{
				nColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			}

			::SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), nColor);
		}
    }
}
#endif