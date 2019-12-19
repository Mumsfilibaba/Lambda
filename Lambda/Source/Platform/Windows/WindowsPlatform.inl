#pragma once
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>

namespace Lambda
{
	//----------------
	//_LMessageBoxImpl
	//----------------

	void _LMessageBoxImpl(const char* pCaption, const char* pText, EMessageBoxType type)
	{
		UINT uType = 0;
		if (type == EMessageBoxType::MESSAGE_BOX_TYPE_ERROR)
		{
			uType = MB_OK | MB_ICONERROR;
		}

		::MessageBoxA(0, pText, pCaption, uType);
	}

    //---------------------
    //Interface for console
    //---------------------

    BOOL g_bHasConsole = FALSE;

    bool _LInitializeConsoleImpl()
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
            LMessageBox("AllocConsole failed", "FATAL ERROR", EMessageBoxType::MESSAGE_BOX_TYPE_ERROR);
            return false;
        }

    }


    void _LSetConsoleColorImpl(EConsoleColor color)
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
        else if (color == EConsoleColor::CONSOLE_COLOR_BLUE)
        {
            nColor = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        }
        else if (color == EConsoleColor::CONSOLE_COLOR_YELLOW)
        {
            nColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        }

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), nColor);
    }


    void _LPrintConsoleImpl(const char* pFormat, va_list args)
    {
        constexpr uint32 MAX_BUFFER_COUNT = 1024;
        static char s_Buffer[MAX_BUFFER_COUNT];

        vsnprintf(s_Buffer, MAX_BUFFER_COUNT, pFormat, args);

        printf(s_Buffer);
        OutputDebugStringA(s_Buffer);
    }


    void _LReleaseConsoleImpl()
    {
        if (g_bHasConsole)
        {
            FreeConsole();
            g_bHasConsole = FALSE;
        }
    }

    //-------------------
    //Interface for mouse
    //-------------------

    void _LShowMouseImpl(bool bVisible)
    {
        if (bVisible)
        {
            ::ShowCursor(TRUE);
        }
        else
        {
            ::ShowCursor(FALSE);
        }
    }


    void _LSetCursorPositionImpl(int32 x, int32 y)
    {
        ::SetCursorPos(x, y);
    }


    void _LGetCursorPositionImpl(int32& outX, int32& outY)
    {
        POINT cursor = {};
        ::GetCursorPos(&cursor);

        outX = cursor.x;
        outY = cursor.y;
    }
}