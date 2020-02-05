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

    class IWindow;
    class IConsoleOutput;

    enum EMessageBox
    {
        MESSAGE_BOX_UNKNOWN     = 0,
        MESSAGE_BOX_OK          = (1 << 1),
        MESSAGE_BOX_ERRORICON   = (1 << 2),
    };

	class LAMBDA_API CommonPlatform
	{
	public:
		LAMBDA_DECL_STATIC_CLASS(CommonPlatform);

        static IConsoleOutput* CreateConsoleOutput()        { return nullptr; }
        static IWindow* CreateWindow(const SWindowDesc&)    { return nullptr; }
		
        static void Init() {}
        static void Release() {}
        
		static void PollEvents() {}
        
        static void MessageBox(const char*, const char*, uint32) {}
        static void OutputDebugString(const char*) {}
	};
}
