#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    class CMacSystem;
}

    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        //---------------
        //CMacAppDelegate
        //---------------
        @interface CMacAppDelegate : NSObject<NSApplicationDelegate>
        {
            Lambda::CMacSystem* m_pSystem;
        }

        - (id) initWithSystem:(Lambda::CMacSystem* ) pSystem;
        @end
    #else
        typedef void CMacAppDelegate;
    #endif
#endif
