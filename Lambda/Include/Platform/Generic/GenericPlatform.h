#pragma once
#include "LambdaCore.h"

#ifdef MessageBox
    #undef MessageBox
#endif

#ifdef OutputDebugString
    #undef OutputDebugString
#endif

#ifdef CreateWindow
    #undef CreateWindow
#endif

namespace Lambda
{
    struct SWindowDesc;

    class IConsoleOutput;
    class CGenericWindow;

    enum EMessageBox
    {
        MESSAGE_BOX_UNKNOWN     = 0,
        MESSAGE_BOX_OK          = (1 << 1),
        MESSAGE_BOX_ERRORICON   = (1 << 2),
    };

	class LAMBDA_API CGenericPlatform
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CGenericPlatform);

        static IConsoleOutput* CreateConsoleOutput()            { return nullptr; }
        static CGenericWindow* CreateWindow(const SWindowDesc&) { return nullptr; }
		
        static void Init() {}
        static void Release() {}
        
		static void PollEvents() {}
        
        static void MessageBox(const char*, const char*, uint32) {}
        static void OutputDebugString(const char*) {}
        
        static uint64 NanoSeconds()     { return 0; }
        static uint64 Ticks()           { return 0; }
        static uint64 TicksPerSecond()  { return 0; }
	};
}
