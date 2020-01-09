#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Engine/Console.h"

    #include "Platform/macOS/MacSystem.h"
    #include "Platform/macOS/MacAppDelegate.h"

    //---------------
    //CMacAppDelegate
    //---------------

    @implementation MacAppDelegate

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (id)initWithSystem:(Lambda::MacSystem*) pSystem
    {
        LAMBDA_ASSERT_PRINT(pSystem != nullptr, "pSystem cannot be nullptr");
        
        self = [super init];
        if (self)
        {
            m_pSystem = pSystem;
        }
        return self;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)applicationDidFinishLaunching:(NSNotification*)notification
    {
        Lambda::Console::PrintLine("Did finish launching");
        
        m_pSystem->SetHasFinishedLaunching(true);
        [NSApp stop:nil];
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)application
    {
        return YES;
    }
    @end
#endif
