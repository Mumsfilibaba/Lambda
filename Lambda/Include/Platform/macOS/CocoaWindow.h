#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    namespace Lambda
    {
        class CMacWindow;
    }

    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        @interface CocoaView : NSView<NSTextInputClient>
        {
            Lambda::CMacWindow* m_pWindow;
        }

        - (id) initWithWindow:(Lambda::CMacWindow*) pWindow;
        @end

        @interface CocoaWindow : NSWindow<NSWindowDelegate>
        {
            Lambda::CMacWindow* m_pWindow;
        }

        - (id) initWithContentRect:(Lambda::CMacWindow*) pWindow :(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag;
        @end
    #else
        class CocoaView;
        class CocoaWindow;
    #endif
#endif
