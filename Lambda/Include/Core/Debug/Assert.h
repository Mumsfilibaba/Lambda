#pragma once
#include "Core/LambdaTypes.h"

namespace Lambda
{
	namespace Assert
	{
		void LAMBDA_API Assert(uint32 line, const char* pFile, const char* pFormat, ...);
	
		void LAMBDA_API EnableDebuggerPrint(bool bEnable);
		void LAMBDA_API EnableDialog(bool bEnable);

		bool LAMBDA_API IsDiablogEnabled();
		bool LAMBDA_API IsDebuggerPrintEnabled();
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
	#define LAMBDA_ASSERT_PRINT(bCondition, ...) \
	if (!(bCondition)) \
	{ \
		Lambda::Assert::Assert(__LINE__, __FILE__, __VA_ARGS__); \
		DebugBreak(); \
	}

	#define LAMBDA_ASSERT(bCondition) LAMBDA_ASSERT_PRINT(bCondition, "Assertion Failed")
#else
	#define LAMBDA_ASSERT_PRINT(bCondition, ...)
	#define LAMBDA_ASSERT(bCondition)
#endif
