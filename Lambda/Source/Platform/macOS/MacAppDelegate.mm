#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #import "Core/Engine/Console.h"

    #import "Platform/macOS/MacSystem.h"
    #import "Platform/macOS/MacAppDelegate.h"

    //---------------
    //CMacAppDelegate
    //---------------

    @implementation CMacAppDelegate

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (id)init:(Lambda::CMacSystem*) pSystem
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
        Lambda::CConsole::PrintLine("Did finish launching");
        
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
