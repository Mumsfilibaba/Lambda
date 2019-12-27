#pragma once
#include "Core/Console.h"

//Asserts
#ifdef LAMBDA_DEBUG
#define LAMBDA_ENABLE_ASSERTS
#endif

#ifdef LAMBDA_ENABLE_ASSERTS
#define LAMBDA_ASSERT_PRINT(x, ...) \
		{ \
			if (!(x)) \
			{ \
				Lambda::CPlatform::MessageBox("FATAL ERROR", "Assertion Failed", Lambda::EMessageBoxType::MESSAGE_BOX_TYPE_ERROR); \
				Lambda::NConsole::SetTextColor(Lambda::EConsoleColor::CONSOLE_COLOR_RED); \
				Lambda::NConsole::Print(__VA_ARGS__); \
				Lambda::NConsole::Reset(); \
				DEBUG_BREAK; \
			} \
		}
#define LAMBDA_ASSERT(x) LAMBDA_ASSERT_PRINT(x, "Assertion Failed\n")
#else
#define LAMBDA_ASSERT_PRINT(x, ...)
#define LAMBDA_ASSERT(x)		
#endif
