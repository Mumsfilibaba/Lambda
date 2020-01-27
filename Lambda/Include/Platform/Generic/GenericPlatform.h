#pragma once
#include "LambdaCore.h"

namespace Lambda
{
    class IConsoleOutput;

    enum EMessageBox
    {
        MESSAGE_BOX_UNKNOWN     = 0,
        MESSAGE_BOX_OK          = (1 << 1),
        MESSAGE_BOX_ERRORICON   = (1 << 2),
    };

	class LAMBDA_API CGenericPlatform
	{
	public:
		LAMBDA_STATIC_CLASS(CGenericPlatform);

		static void Init() {}
        static void Release() {}
        
        static IConsoleOutput* CreateConsoleOutput() { return nullptr; }
        
		static void PollEvents() {}
        
        static void MessageBox(const char*, const char*, uint32) {}
        static void OutputDebugString(const char*) {}
        
        static uint64 NanoSeconds()     { return 0; }
        static uint64 Ticks()           { return 0; }
        static uint64 TicksPerSecond()  { return 0; }
	};
}
