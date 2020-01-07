#pragma once
#define MAX_MESSAGE_LENGTH 1024

namespace Lambda
{
	namespace Assert
	{
		//Enables a debugbreak if a debugger is attached and prints a message to the console and a messagebox
		void AssertWithMessage(int nLine, const char* pFile, const char* pFormat, ...);
    
        void SetShowDialogOnAssert(bool bShowDialogOnAssert);
        void SetWriteConsoleOnAssert(bool bWriteConsoleOnAssert);
        
        bool ShowDialogOnAssert();
        bool WriteConsoleOnAssert();
	}
}

//DebugBreak (Compiler based)
#if defined(LAMBDA_VISUAL_STUDIO)
    #define DebugBreak(...) __debugbreak()
#else
    #include <stdlib.h>
    #define DebugBreak(...) abort()
#endif

//Asserts
#ifdef LAMBDA_DEBUG
	#define LAMBDA_ENABLE_ASSERTS
#endif

#ifdef LAMBDA_ENABLE_ASSERTS
	#define LAMBDA_ASSERT_PRINT(bCondition, ...) \
		{ \
			if (!(bCondition)) \
			{ \
                Lambda::Assert::AssertWithMessage(__LINE__, __FILE__, __VA_ARGS__); \
                DebugBreak(); \
			} \
		}
	#define LAMBDA_ASSERT(bCondition) LAMBDA_ASSERT_PRINT(bCondition, "Assertion Failed")
#else
	#define LAMBDA_ASSERT_PRINT(bCondition, ...)
	#define LAMBDA_ASSERT(bCondition)		
#endif
