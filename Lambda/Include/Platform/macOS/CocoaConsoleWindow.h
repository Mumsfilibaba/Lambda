#pragma once

#if defined(LAMBDA_PLAT_MACOS)
    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        @interface CocoaConsoleWindow : NSWindow<NSWindowDelegate>
        {
        }
        @end
    #else
        class NSTextView;
        class NSDictionary;
        class NSScrollView;
        class CocoaConsoleWindow;
    #endif
#endif
