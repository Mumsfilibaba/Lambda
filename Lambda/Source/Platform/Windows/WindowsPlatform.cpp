#include "LambdaPch.h"
#include "Core/Console.h"
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

namespace Lambda
{
    //---------------------
    //Interface for console
    //---------------------

    BOOL g_bHasConsole = FALSE;

    bool InitializeConsole()
    {
        if (g_bHasConsole)
        {
            return true;
        }

        BOOL bResult = ::AllocConsole();
        if (bResult)
        {
            //Attach the console and redirect the standard output to the new console
            AttachConsole(GetCurrentProcessId());
            freopen("CONIN$", "r", stdin);
            freopen("CONOUT$", "w", stderr);
            freopen("CONOUT$", "w", stdout);

            //Set title of console
            if (!SetConsoleTitleA("Lambda Engine Debug Output"))
            {
                printf("SetConsoleTitle failed (%d)\n", GetLastError());
                return false;
            }

            //TODO: A message should probably be sent to the main application so that it can exit

            g_bHasConsole = TRUE;
            return true;
        }
        else
        {
            MessageBoxA(0, "AllocConsole failed", "FATAL ERROR", MB_OK);
            return false;
        }
        
    }


    void SetConsoleColor(EConsoleColor color)
    {
        WORD nColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        if (color == CONSOLE_COLOR_RED)
        {
            nColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
        }
        else if (color == CONSOLE_COLOR_GREEN)
        {
            nColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        }
        else if (color == CONSOLE_COLOR_BLUE)
        {
            nColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        }
        else if (color == CONSOLE_COLOR_YELLOW)
        {
            nColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        }

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), nColor);
    }


    void PrintConsole(const char* pFormat, ...)
    {
        constexpr uint32 MAX_BUFFER_COUNT = 1024;
        static char s_Buffer[MAX_BUFFER_COUNT];

        va_list args;
        va_start(args, pFormat);
        vsnprintf(s_Buffer, MAX_BUFFER_COUNT, pFormat, args);
        va_end(args);

        printf(s_Buffer);
        OutputDebugStringA(s_Buffer);
    }


    void ReleaseConsole()
    {
        if (g_bHasConsole)
        {
            FreeConsole();
            g_bHasConsole = FALSE;
        }
    }
}