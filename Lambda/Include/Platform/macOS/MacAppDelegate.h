#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    class MacSystem;
}

    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        //--------------
        //MacAppDelegate
        //--------------

        @interface MacAppDelegate : NSObject<NSApplicationDelegate>
        {
            Lambda::MacSystem* m_pSystem;
        }

        - (id) initWithSystem:(Lambda::MacSystem* ) pSystem;
        @end

    #else
        class MacAppDelegate;
    #endif
#endif
