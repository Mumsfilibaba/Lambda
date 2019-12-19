#pragma once
#include "Platform.h"

//Asserts
#ifdef LAMBDA_DEBUG
#define LAMBDA_ENABLE_ASSERTS
#endif

#ifdef LAMBDA_ENABLE_ASSERTS
#define LAMBDA_ASSERT_PRINT(x, ...) \
		{ \
			if (!(x)) \
			{ \
				LMessageBox("FATAL ERROR", "Assertion Failed", Lambda::EMessageBoxType::MESSAGE_BOX_TYPE_ERROR); \
				LSetConsoleColor(Lambda::EConsoleColor::CONSOLE_COLOR_RED); \
				LPrintConsole(__VA_ARGS__); \
				LSetConsoleColor(Lambda::EConsoleColor::CONSOLE_COLOR_WHITE); \
				DEBUG_BREAK; \
		} \
	}
#define LAMBDA_ASSERT(x) LAMBDA_ASSERT_PRINT(x, "Assertion Failed\n")
#else
#define LAMBDA_ASSERT_PRINT(x, ...)
#define LAMBDA_ASSERT(x)		
#endif