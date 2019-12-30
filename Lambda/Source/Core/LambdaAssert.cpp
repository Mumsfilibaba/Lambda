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
			char message[MAX_MESSAGE_LENGTH];
			memset(message, 0, sizeof(message));
			
			va_list args;
			va_start(args, pFormat);
			vsnprintf(message, MAX_MESSAGE_LENGTH, pFormat, args);
			va_end(args);

			IEngine* pEngine = IEngine::Get();
			if (pEngine)
			{
				IConsole* pConsole = pEngine->GetConsole();
				if (pConsole)
				{
					pConsole->PrintLine("Assertion Failed: %s", message);
				}
			}

			CPlatform::MessageBox("ASSERTION FAILED", message, EMessageBoxType::MESSAGE_BOX_TYPE_ERROR);

#if defined(LAMBDA_VISUAL_STUDIO)
			__debugbreak();
#else
			assert(false);
#endif
		}
	}
}