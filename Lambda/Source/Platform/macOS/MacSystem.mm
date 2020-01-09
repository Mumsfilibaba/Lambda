#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #include "Core/Engine/Console.h"
    #include "Core/Engine/Engine.h"
    
    #include "Platform/macOS/MacSystem.h"
    #include "Platform/macOS/MacKeyboard.h"
    #include "Platform/macOS/MacPlatform.h"

namespace Lambda
{
    //---------
    //MacSystem
    //---------
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    ISystem* MacSystem::Create()
    {
        ISystem* pSystem = DBG_NEW MacSystem();
        return pSystem;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    MacSystem::MacSystem()
        : ISystem()
    {
        //Initialize pointers to nullptr
        m_pAppDelegate = nullptr;
        
        //Init rest of the members
        m_bHasFinishedLaunching = false;
        m_bShouldExit = false;
        
        //Init platform
        MacKeyboard::Initialize();
        
        //Create static application instance
        [NSApplication sharedApplication];
        
        //Create application delegate
        MacAppDelegate* pAppDelegate = [[MacAppDelegate alloc] initWithSystem:this];
        if (pAppDelegate == nil)
        {
            Console::PrintLine("Failed to create AppDelegate");
        }
        else
        {
            //Set delegate
            m_pAppDelegate = pAppDelegate;
            [NSApp setDelegate:m_pAppDelegate];
            
            //Create a menu bar
            CreateMenuBar();
            
            //Run the application, the application will be stopped when it has finished launching
            [NSApp run];
            
            Console::PrintLine("Created AppDelegate");
        }
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    MacSystem::~MacSystem()
    {
        [m_pAppDelegate release];
    }
    
    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    IWindow* MacSystem::CreateWindow(const char* pTitle, uint32 nWidth, uint32 nHeight)
    {
        //Check if the applicatio has finished launching and then create window
        if (HasFinishedLaunching())
        {
            MacWindow* pWindow = DBG_NEW MacWindow(this, pTitle, nWidth, nHeight);
            return pWindow;
        }
        
        //TODO: Print error
        return nullptr;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void MacSystem::ProcessEvents()
    {
        LAMBDA_ASSERT_PRINT(HasFinishedLaunching(), "Application did not finish launching before proccessing events");
        
        //Get events if finished otherwise run
        while (true)
        {
            NSEvent* pEvent = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
            if (pEvent == nil)
            {
                break;
            }
            
            [NSApp sendEvent:pEvent];
        }
        
        if (ShouldExit())
        {
            Engine& engine = Engine::Get();
            engine.Exit(0);
        }
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void MacSystem::OnWindowClose(MacWindow*)
    {
        m_bShouldExit = true;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    void MacSystem::CreateMenuBar()
    {
        size_t i;
        NSString* appName = nil;
        NSDictionary* bundleInfo = [[NSBundle mainBundle] infoDictionary];
        NSString* nameKeys[] =
        {
            @"CFBundleDisplayName",
            @"CFBundleName",
            @"CFBundleExecutable",
        };
        
        // Try to figure out what the calling application is called
        for (i = 0;  i < sizeof(nameKeys) / sizeof(nameKeys[0]);  i++)
        {
            id name = bundleInfo[nameKeys[i]];
            if (name && [name isKindOfClass:[NSString class]] && ![name isEqualToString:@""])
            {
                appName = name;
                break;
            }
        }
        
        if (!appName)
        {
            appName = @"Lambda Engine";
        }
        
        NSMenu* bar = [[NSMenu alloc] init];
        [NSApp setMainMenu:bar];
        
        NSMenuItem* appMenuItem = [bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
        NSMenu* appMenu = [[NSMenu alloc] init];
        [appMenuItem setSubmenu:appMenu];
        
        [appMenu addItemWithTitle:[NSString stringWithFormat:@"About %@", appName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
        [appMenu addItem:[NSMenuItem separatorItem]];
        
        NSMenu* servicesMenu = [[NSMenu alloc] init];
        [NSApp setServicesMenu:servicesMenu];
        [[appMenu addItemWithTitle:@"Services" action:NULL keyEquivalent:@""] setSubmenu:servicesMenu];
        [servicesMenu release];
        [appMenu addItem:[NSMenuItem separatorItem]];
        [appMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", appName] action:@selector(hide:) keyEquivalent:@"h"];
        [[appMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"] setKeyEquivalentModifierMask:NSEventModifierFlagOption | NSEventModifierFlagCommand];
        [appMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
        [appMenu addItem:[NSMenuItem separatorItem]];
        [appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName] action:@selector(terminate:) keyEquivalent:@"q"];
        
        NSMenuItem* windowMenuItem = [bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
        [bar release];
        
        NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
        [NSApp setWindowsMenu:windowMenu];
        [windowMenuItem setSubmenu:windowMenu];
        
        [windowMenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
        [windowMenu addItemWithTitle:@"Zoom" action:@selector(performZoom:) keyEquivalent:@""];
        [windowMenu addItem:[NSMenuItem separatorItem]];
        [windowMenu addItemWithTitle:@"Bring All to Front" action:@selector(arrangeInFront:) keyEquivalent:@""];
        
        // TODO: Make this appear at the bottom of the menu (for consistency)
        [windowMenu addItem:[NSMenuItem separatorItem]];
        [[windowMenu addItemWithTitle:@"Enter Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"] setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
        
        // Prior to Snow Leopard, we need to use this oddly-named semi-private API
        // to get the application menu working properly.
        SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
        [NSApp performSelector:setAppleMenuSelector withObject:appMenu];
    }
}
#endif

