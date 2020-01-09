#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    namespace Lambda
    {
        class MacWindow;
    }

    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        //---------
        //CocoaView
        //---------
        @interface CocoaView : NSView<NSTextInputClient>
        {
            Lambda::MacWindow* m_pWindow;
        }

        - (id) initWithWindow:(Lambda::MacWindow*) pWindow;
        @end

        //------------
        //CocoaWindow
        //------------
        @interface CocoaWindow : NSWindow<NSWindowDelegate>
        {
            Lambda::MacWindow* m_pWindow;
        }

        - (id) initWithContentRect:(Lambda::MacWindow*) pWindow :(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag;
        @end
    #else
        class CocoaView;
        class CocoaWindow;
    #endif
#endif
