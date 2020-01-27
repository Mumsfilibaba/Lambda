#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Platform/macOS/MacAppDelegate.h"

    @implementation MacAppDelegate

    - (void)applicationDidFinishLaunching:(NSNotification*)notification
    {
        [NSApp stop:nil];
    }

    - (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)application
    {
        return NO;
    }
    @end
#endif
