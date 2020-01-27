#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        @interface MacAppDelegate : NSObject<NSApplicationDelegate>
        {
        }
        @end

    #else
        class MacAppDelegate;
    #endif
#endif
