#pragma once
#include "Core/Console.h"
#include "Core/Platform.h"

//Asserts
#ifdef LAMBDA_DEBUG
	#define LAMBDA_ENABLE_ASSERTS
#endif

#ifdef LAMBDA_ENABLE_ASSERTS
	#ifdef MessageBox
		#undef MessageBox
	#endif

	#define LAMBDA_ASSERT_PRINT(x, ...) \
		{ \
			if (!(x)) \
			{ \
				Lambda::Platform::MessageBox("FATAL ERROR", "Assertion Failed", Lambda::EMessageBoxType::MESSAGE_BOX_TYPE_ERROR); \
				Lambda::Console::SetTextColor(Lambda::EConsoleColor::CONSOLE_COLOR_RED); \
				Lambda::Console::Print(__VA_ARGS__); \
				Lambda::Console::Reset(); \
				DEBUG_BREAK; \
			} \
		}
	#define LAMBDA_ASSERT(x) LAMBDA_ASSERT_PRINT(x, "Assertion Failed\n")
#else
	#define LAMBDA_ASSERT_PRINT(x, ...)
	#define LAMBDA_ASSERT(x)		
#endif
