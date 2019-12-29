#include "LambdaPch.h"
#include "Core/Console.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsPlatform.h"

	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	//---------------
	//SWindowsConsole
	//---------------
	struct SWindowsConsoleState
	{
		bool bHasConsole = false;
	} g_WindowsConsoleState;


    //-------
    //Console
    //-------
    namespace Console
    {
        bool Initialize()
		{
			//If we already have called initialize, return
			if (g_WindowsConsoleState.bHasConsole)
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

				g_WindowsConsoleState.bHasConsole = true;
				return true;
			}
			else
			{
				WindowsPlatform::MessageBox("AllocConsole failed", "FATAL ERROR", EMessageBoxType::MESSAGE_BOX_TYPE_ERROR);
				return false;
			}
		}


		bool HasConsole()
		{
			return g_WindowsConsoleState.bHasConsole;
		}


        void Print(const char* pFormat, ...)
		{
			constexpr uint32 MAX_BUFFER_COUNT = 1024;
			static char s_Buffer[MAX_BUFFER_COUNT];

			va_list args;
			va_start(args, pFormat);
			vsnprintf(s_Buffer, MAX_BUFFER_COUNT, pFormat, args);
			va_end(args, pFormat);

			printf(s_Buffer);
			::OutputDebugStringA(s_Buffer);
		}


        void Release()
		{
			if (g_WindowsConsoleState.bHasConsole)
			{
				::FreeConsole();
				g_WindowsConsoleState.bHasConsole = false;
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