#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_WINDOWS)
	#include "Platform/Windows/WindowsConsoleOutput.h"	

namespace Lambda
{
	CWindowsConsoleOutput::CWindowsConsoleOutput()
		: IConsoleOutput(),
		m_hOutput(0),
		m_CurrentColor(EConsoleColor::CONSOLE_COLOR_WHITE)
	{
		//Get the current consolewindow, if this is null we have no console
		HWND hWnd = ::GetConsoleWindow();
		if (hWnd == NULL)
		{
			BOOL bResult = ::AllocConsole();
			if (!bResult)
				return;
		}

		m_hOutput = ::GetStdHandle(STD_OUTPUT_HANDLE);
		CWindowsConsoleOutput::SetTextColor(EConsoleColor::CONSOLE_COLOR_WHITE);
	}

	CWindowsConsoleOutput::~CWindowsConsoleOutput()
	{
		Hide();
	}

	void CWindowsConsoleOutput::Show()
	{
		if (m_hOutput)
		{
			DWORD nProcessID = ::GetCurrentProcessId();
			/*BOOL bResult = */::AttachConsole(nProcessID);
		}
	}

	void CWindowsConsoleOutput::Hide()
	{
		if (m_hOutput)
			::FreeConsole();
	}

	void CWindowsConsoleOutput::SetTitle(const char* pTitle)
	{
		if (m_hOutput)
			::SetConsoleTitleA(pTitle);
	}

	void CWindowsConsoleOutput::SetTextColor(EConsoleColor color)
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

	void CWindowsConsoleOutput::Write(const char* pFormat, ...)
	{
		va_list args;
		va_start(args, pFormat);
		CWindowsConsoleOutput::WriteV(pFormat, args);
		va_end(args);
	}

	void CWindowsConsoleOutput::WriteV(const char* pFormat, va_list args)
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

	void CWindowsConsoleOutput::Clear()
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
				return;

			// Get the current text attribute.
			if (!::GetConsoleScreenBufferInfo(m_hOutput, &csbi))
				return;

			// Set the buffer's attributes accordingly.
			if (!::FillConsoleOutputAttribute(m_hOutput, csbi.wAttributes, nSize, coordScreen, &nWritten))
				return;

			// Put the cursor at its home coordinates.
			SetConsoleCursorPosition(m_hOutput, coordScreen);
		}
	}
}
#endif