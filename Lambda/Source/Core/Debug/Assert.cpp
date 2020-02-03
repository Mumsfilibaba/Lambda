#include "LambdaPch.h"

#include "Core/Debug/Assert.h"
#include "Platform/Platform.h"

#ifdef ERROR
	#undef ERROR
#endif

#include <stdio.h>

namespace Lambda
{
	namespace Assert
	{
		bool g_EnableDialog = true;
		bool g_EnableDebuggerPrint = true;

		void EnableDebuggerPrint(bool bEnable)
		{
			g_EnableDebuggerPrint = bEnable;
		}

		void EnableDialog(bool bEnable)
		{
			g_EnableDialog = bEnable;
		}

		bool IsDiablogEnabled()
		{
			return g_EnableDialog;
		}
		
		bool IsDebuggerPrintEnabled()
		{
			return g_EnableDebuggerPrint;
		}

		void Assert(uint32 line, const char* pFile, const char* pFormat, ...)
		{
			constexpr uint32 MAX_MESSAGE_LENGTH = 512;
			static char message[MAX_MESSAGE_LENGTH];
			static char assertMessage[MAX_MESSAGE_LENGTH];

			if (!IsDiablogEnabled() && !IsDebuggerPrintEnabled())
			{
				return;
			}

			//Clear buffers
			memset(message, 0, sizeof(message));
			memset(assertMessage, 0, sizeof(assertMessage));

			//Print user message to buffer
			va_list args;
			va_start(args, pFormat);
			vsnprintf(message, MAX_MESSAGE_LENGTH, pFormat, args);
			va_end(args);

			//Print message to buffer
			snprintf(assertMessage, MAX_MESSAGE_LENGTH, "Assertion Failed\nLine: %d\nFile:%s\nMessage: %s", line, pFile, message);

			if (IsDiablogEnabled())
			{
                Platform::MessageBox("Assert Failed", assertMessage, MESSAGE_BOX_OK | MESSAGE_BOX_ERRORICON);
			}

			if (IsDebuggerPrintEnabled())
			{
                Platform::OutputDebugString(assertMessage);
                Platform::OutputDebugString("\n");
			}
		}
	}
}
