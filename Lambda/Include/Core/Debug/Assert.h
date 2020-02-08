#pragma once
#include "Core/LambdaTypes.h"

namespace Lambda
{
	namespace Assert
	{
		LAMBDA_API void Assert(uint32 line, const char* pFile, const char* pFormat, ...);
	
		LAMBDA_API void EnableDebuggerPrint(bool bEnable);
		LAMBDA_API void EnableDialog(bool bEnable);

		LAMBDA_API bool IsDiablogEnabled();
		LAMBDA_API bool IsDebuggerPrintEnabled();
	}
}

//DebugBreak
#ifdef LAMBDA_VISUAL_STUDIO
	#define DebugBreak(...) __debugbreak()
#else
    #include <stdlib.h>
	#define DebugBreak(...) abort()
#endif

//Enable asserts
#ifndef LAMBDA_NO_ASSERTS
	#ifdef LAMBDA_DEBUG
		#define LAMBDA_ENABLE_ASSERTS
	#endif
#endif

//Asserts
#ifdef LAMBDA_ENABLE_ASSERTS
	#define LAMBDA_ASSERT_MSG(bCondition, ...) \
	if (!(bCondition)) \
	{ \
		Lambda::Assert::Assert(__LINE__, __FILE__, __VA_ARGS__); \
		DebugBreak(); \
	}

	#define LAMBDA_ASSERT(bCondition) LAMBDA_ASSERT_MSG(bCondition, "Assertion Failed")
#else
	#define LAMBDA_ASSERT_MSG(bCondition, ...)
	#define LAMBDA_ASSERT(bCondition)
#endif
