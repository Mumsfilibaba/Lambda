#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        @interface CMacAppDelegate : NSObject<NSApplicationDelegate>
        {
        }
        @end

    #else
        class CMacAppDelegate;
    #endif
#endif
