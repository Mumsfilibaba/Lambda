#include "LambdaPch.h"

#include "Core/LambdaAssert.h"
#include "Core/Platform.h"

#include "Core/Engine/Engine.h"
#include "Core/Engine/Console.h"

//For other platforms we just use assert(false) for now
#if !defined(LAMBDA_VISUAL_STUDIO)
	#include <assert.h>
#endif

namespace Lambda
{
	namespace Assert
	{
		//------
		//Assert
		//------

		void AssertWithMessage(const char* pFormat, ...)
		{
			//Clear messagebuffer
			static char message[MAX_MESSAGE_LENGTH];
			memset(message, 0, sizeof(message));
			
			//Print message to buffer
			va_list args;
			va_start(args, pFormat);
			vsnprintf(message, MAX_MESSAGE_LENGTH, pFormat, args);
			va_end(args);

			//Print to console
			IConsole* pConsole = CConsole::Get();
			if (pConsole)
			{
				pConsole->SetTextColor(EConsoleColor::CONSOLE_COLOR_RED);
				pConsole->PrintLine("Assertion Failed: %s", message);
				pConsole->Reset();
			}

			//Print messagebox
            Platform::MessageBox("ASSERTION FAILED", message, EMessageBoxType::MESSAGE_BOX_TYPE_ERROR);

			//Break in debugger
#if defined(LAMBDA_VISUAL_STUDIO)
			__debugbreak();
#else
			assert(false);
#endif
		}
	}
}
