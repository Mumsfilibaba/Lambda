#pragma once
#include "Platform/Generic/GenericMisc.h"

#ifdef LAMBDA_PLAT_MACOS

namespace Lambda
{
    class LAMBDA_API CMacMisc : public CGenericMisc
    {
    public:
        LAMBDA_STATIC_CLASS(CMacMisc);

        static void MessageBox(const char* pCaption, const char* pText, uint32 type);
        static void DebuggerOutput(const char* pMessage);
    };

    typedef CMacMisc PlatformMisc;
}
#endif
