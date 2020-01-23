#pragma once
#include "Platform/Generic/GenericApplication.h"

#ifdef LAMBDA_PLAT_MACOS

namespace Lambda
{
    class LAMBDA_API CMacApplication : public CGenericApplication
    {
    public:
        LAMBDA_STATIC_CLASS(CMacApplication);

        static void Init();
        static void PollEvents();
    };

    typedef CMacApplication PlatformApplication;
}
#endif
