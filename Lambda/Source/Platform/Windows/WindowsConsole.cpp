#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsConsole.h"
	
	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

namespace Lambda
{
	//---------------
	//WindowsConsole
	//---------------

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	IConsole* WindowsConsole::CreateWindowsConsole()
	{
		IConsole* pWindowsConsole = DBG_NEW WindowsConsole();
		return pWindowsConsole;
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	WindowsConsole::WindowsConsole()
		: IConsole(),
		m_hOutput(0),
		m_CurrentColor(EConsoleColor::CONSOLE_COLOR_WHITE)
	{
		//Get the current consolewindow, if this is null we have no console
		HWND hWnd = ::GetConsoleWindow();
		if (hWnd == NULL)
		{
			BOOL bResult = ::AllocConsole();
			if (bResult)
			{
				DWORD nProcessID = ::GetCurrentProcessId();
				::AttachConsole(nProcessID);

				//Get output handle after attaching a console
				m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);

				//Set default values
				WindowsConsole::SetTitle("Lambda Engine Debug Output");
				WindowsConsole::Reset();
			}
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	WindowsConsole::~WindowsConsole()
	{
		if (m_hOutput)
		{
			::FreeConsole();
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::SetTitle(const char* pTitle)
	{
		if (m_hOutput)
		{
			::SetConsoleTitleA(pTitle);
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::SetTextColor(EConsoleColor color)
	{
		if (m_hOutput)
		{
			//Inital color is white
			WORD nColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
			if (color == EConsoleColor::CONSOLE_COLOR_RED)
				nColor = FOREGROUND_RED | FOREGROUND_INTENSITY;
			else if (color == EConsoleColor::CONSOLE_COLOR_GREEN)
				nColor = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
			else if (color == EConsoleColor::CONSOLE_COLOR_YELLOW)
				nColor = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;

			::SetConsoleTextAttribute(m_hOutput, nColor);
			m_CurrentColor = color;
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::Print(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		WindowsConsole::PrintV(pFormat, args);
		va_end(args);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::PrintV(const char* pFormat, va_list args)
	{
		if (m_hOutput)
		{
			static char buffer[MAX_OUTPUT_CHARS];
			memset(buffer, 0, sizeof(buffer));

			DWORD nLength = vsnprintf(buffer, MAX_OUTPUT_CHARS, pFormat, args);

			DWORD nWritten = 0;
			::WriteConsoleA(m_hOutput, buffer, nLength, &nWritten, 0);
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::PrintLine(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		WindowsConsole::PrintLineV(pFormat, args);
		va_end(args);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::PrintLineV(const char* pFormat, va_list args)
	{
		if (m_hOutput)
		{
			static char buffer[MAX_OUTPUT_CHARS];
			DWORD nLength = vsnprintf(buffer, MAX_OUTPUT_CHARS, pFormat, args);

			DWORD nWritten = 0;
			::WriteConsoleA(m_hOutput, buffer, nLength, &nWritten, 0);
			::WriteConsoleA(m_hOutput, "\n", 1, &nWritten, 0);
		}
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::Reset()
	{
		WindowsConsole::SetTextColor(EConsoleColor::CONSOLE_COLOR_WHITE);
	}

	/*////////////////////////////////////////////////////////////////////////////////////////////////*/
	void WindowsConsole::Clear()
	{
		if (m_hOutput)
		{
			// Get the number of character cells in the current buffer. 
			CONSOLE_SCREEN_BUFFER_INFO csbi = {};
			if (!::GetConsoleScreenBufferInfo(m_hOutput, &csbi))
			{
				return;
			}

			// Fill the entire screen with blanks.
			DWORD nWritten;
			COORD coordScreen = { 0, 0 };
			
			DWORD nSize = csbi.dwSize.X * csbi.dwSize.Y;
			if (!::FillConsoleOutputCharacterA(m_hOutput, (TCHAR)' ', nSize, coordScreen, &nWritten))
			{
				return;
			}

			// Get the current text attribute.
			if (!::GetConsoleScreenBufferInfo(m_hOutput, &csbi))
			{
				return;
			}

			// Set the buffer's attributes accordingly.
			if (!::FillConsoleOutputAttribute(m_hOutput, csbi.wAttributes, nSize, coordScreen, &nWritten))
			{
				return;
			}

			// Put the cursor at its home coordinates.
			SetConsoleCursorPosition(m_hOutput, coordScreen);
		}
	}
}
#endif
