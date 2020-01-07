#pragma once
#include "LambdaCore.h"

#if defined(LAMBDA_PLAT_MACOS)
    namespace Lambda
    {
        class CMacWindow;
    }

    #if defined(__OBJC__)
        #include <Cocoa/Cocoa.h>

        //----------
        //CCocoaView
        //----------
        @interface CCocoaView : NSView<NSTextInputClient>
        {
            Lambda::CMacWindow* m_pWindow;
        }

        - (id) initWithWindow:(Lambda::CMacWindow*) pWindow;
        @end

        //------------
        //CCocoaWindow
        //------------
        @interface CCocoaWindow : NSWindow<NSWindowDelegate>
        {
            Lambda::CMacWindow* m_pWindow;
        }

        - (id) initWithContentRect:(Lambda::CMacWindow*) pWindow :(NSRect)contentRect styleMask:(NSWindowStyleMask)style backing:(NSBackingStoreType)backingStoreType defer:(BOOL)flag;
        @end
    #else
        class CCocoaView;
        class CCocoaWindow;
    #endif
#endif
