#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/macOS/MacPlatform.h"
    #include "Platform/macOS/MacTime.h"
    #include "Platform/macOS/MacWindow.h"
    #include "Platform/macOS/MacConsoleOutput.h"
    
    #include <Cocoa/Cocoa.h>
    #include <Foundation/Foundation.h>
    #include <CoreServices/CoreServices.h>

namespace Lambda
{
    CMacAppDelegate* MacPlatform::s_pAppDelegate = nullptr;

    IConsoleOutput* MacPlatform::CreateConsoleOutput()
    {
        return DBG_NEW CMacConsoleOutput();
    }

    IWindow* MacPlatform::CreateWindow(const SWindowDesc& desc)
    {
        return DBG_NEW CMacWindow(desc);
    }

    void MacPlatform::CreateMenuBar()
    {
        size_t i;
        NSString* pAppName = nil;
        NSDictionary* pBundleInfo = [[NSBundle mainBundle] infoDictionary];
        NSString* ppNameKeys[] =
        {
            @"CFBundleDisplayName",
            @"CFBundleName",
            @"CFBundleExecutable",
        };
        
        // Try to figure out what the calling application is called
        for (i = 0;  i < sizeof(ppNameKeys) / sizeof(ppNameKeys[0]);  i++)
        {
            id name = pBundleInfo[ppNameKeys[i]];
            if (name && [name isKindOfClass:[NSString class]] && ![name isEqualToString:@""])
            {
                pAppName = name;
                break;
            }
        }
        
        if (!pAppName)
            pAppName = @"Lambda Engine";

        NSMenu* pMenuBar = [[NSMenu alloc] init];
        [NSApp setMainMenu:pMenuBar];
        
        NSMenuItem* pAppMenuItem = [pMenuBar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
        NSMenu* pAppMenu = [[NSMenu alloc] init];
        [pAppMenuItem setSubmenu:pAppMenu];
        
        [pAppMenu addItemWithTitle:[NSString stringWithFormat:@"About %@", pAppName] action:@selector(orderFrontStandardAboutPanel:) keyEquivalent:@""];
        [pAppMenu addItem:[NSMenuItem separatorItem]];
        
        NSMenu* pServicesMenu = [[NSMenu alloc] init];
        [NSApp setServicesMenu:pServicesMenu];
        [[pAppMenu addItemWithTitle:@"Services" action:NULL keyEquivalent:@""] setSubmenu:pServicesMenu];
        [pServicesMenu release];
        [pAppMenu addItem:[NSMenuItem separatorItem]];
        [pAppMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", pAppName] action:@selector(hide:) keyEquivalent:@"h"];
        [[pAppMenu addItemWithTitle:@"Hide Others" action:@selector(hideOtherApplications:) keyEquivalent:@"h"] setKeyEquivalentModifierMask:NSEventModifierFlagOption | NSEventModifierFlagCommand];
        [pAppMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:) keyEquivalent:@""];
        [pAppMenu addItem:[NSMenuItem separatorItem]];
        [pAppMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", pAppName] action:@selector(terminate:) keyEquivalent:@"q"];
        
        NSMenuItem* pWindowMenuItem = [pMenuBar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
        [pMenuBar release];
        
        NSMenu* pWindowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
        [NSApp setWindowsMenu:pWindowMenu];
        [pWindowMenuItem setSubmenu:pWindowMenu];
        
        [pWindowMenu addItemWithTitle:@"Minimize" action:@selector(performMiniaturize:) keyEquivalent:@"m"];
        [pWindowMenu addItemWithTitle:@"Zoom" action:@selector(performZoom:) keyEquivalent:@""];
        [pWindowMenu addItem:[NSMenuItem separatorItem]];
        [pWindowMenu addItemWithTitle:@"Bring All to Front" action:@selector(arrangeInFront:) keyEquivalent:@""];
        
        [pWindowMenu addItem:[NSMenuItem separatorItem]];
        [[pWindowMenu addItemWithTitle:@"Enter Full Screen" action:@selector(toggleFullScreen:) keyEquivalent:@"f"] setKeyEquivalentModifierMask:NSEventModifierFlagControl | NSEventModifierFlagCommand];
        
        SEL setAppleMenuSelector = NSSelectorFromString(@"setAppleMenu:");
        [NSApp performSelector:setAppleMenuSelector withObject:pAppMenu];
    }

    void MacPlatform::Init()
    {
        //Create static application instance
        [NSApplication sharedApplication];
        
        CMacAppDelegate* pAppDelegate = [[CMacAppDelegate alloc] init];
        if (pAppDelegate != nil)
        {
            s_pAppDelegate = pAppDelegate;
            [NSApp setDelegate:s_pAppDelegate];
            
            CreateMenuBar();
        }

        [NSApp run];
        
        MacTime::Init();
    }

    void MacPlatform::Release()
    {
        [s_pAppDelegate release];
    }

    void MacPlatform::PollEvents()
    {
        while (true)
        {
            NSEvent* pEvent = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
            if (pEvent == nil)
                break;
            
            [NSApp sendEvent:pEvent];
        }
    }

    void MacPlatform::MessageBox(const char* pCaption, const char* pText, uint32 type)
    {
        CFStringRef text    = CFStringCreateWithCString(0, pText, strlen(pText));
        CFStringRef caption = CFStringCreateWithCString(0, pCaption, strlen(pCaption));

        CFOptionFlags result;
        CFOptionFlags uType = kCFUserNotificationNoteAlertLevel;
        if (type == EMessageBox::MESSAGE_BOX_ERRORICON)
            uType = kCFUserNotificationStopAlertLevel;
        
        CFUserNotificationDisplayAlert(0, uType, 0, 0, 0, caption, text, 0, 0, 0, &result);

        CFRelease(caption);
        CFRelease(text);
    }

    void MacPlatform::OutputDebugString(const char* pMessage)
    {
        NSLog(@"%s", pMessage);
    }

    EMouseButton MacPlatform::ConvertMouseButton(uint32 button)
    {
        if (button == 0)
            return EMouseButton::MOUSEBUTTON_LEFT;
        else if (button == 1)
            return EMouseButton::MOUSEBUTTON_RIGHT;
        else if (button == 2)
            return EMouseButton::MOUSEBUTTON_MIDDLE;
        else
            return EMouseButton::MOUSEBUTTON_UNKNOWN;
    }
}
#endif
