#include "LambdaPch.h"

#if defined(LAMBDA_PLAT_MACOS)
    #import "Platform/macOS/MacWindow.h"
    #import "Platform/macOS/MacPlatform.h"
    #import "Platform/macOS/CocoaWindow.h"

//----------
//CCocoaView
//----------

@implementation CCocoaView

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (id) initWithWindow:(Lambda::CMacWindow*) pWindow
    {
        self = [super init];
        if (self)
        {
            m_pWindow = pWindow;
        }
        
        return self;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)canBecomeKeyView
    {
        return YES;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)acceptsFirstResponder
    {
        return YES;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (NSArray*)validAttributesForMarkedText
    {
        return [NSArray array];
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)hasMarkedText
    {
        return NO;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)keyDown:(NSEvent*) event
    {
        uint32 keycode   = [event keyCode];
        uint32 flags     = [event modifierFlags];
        uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
        m_pWindow->OnKeydown(Lambda::MacPlatform::ConvertFromVirtualKey(keycode), modifiers);
        
        [self interpretKeyEvents:@[event]];
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)keyUp:(NSEvent*) event
   {
       uint32 keycode   = [event keyCode];
       uint32 flags     = [event modifierFlags];
       uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
       m_pWindow->OnKeyup(Lambda::MacPlatform::ConvertFromVirtualKey(keycode), modifiers);
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)mouseDown:(NSEvent*) event
   {
       uint32 flags     = [event modifierFlags];
       uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
       m_pWindow->OnMouseButtonDown(Lambda::EMouseButton::MOUSEBUTTON_LEFT, modifiers);
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)mouseDragged:(NSEvent*) event
   {
       [self mouseMoved:event];
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)mouseUp:(NSEvent*) event
   {
       uint32 flags     = [event modifierFlags];
       uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
       m_pWindow->OnMouseButtonUp(Lambda::EMouseButton::MOUSEBUTTON_LEFT, modifiers);
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)mouseMoved:(NSEvent*) event
   {
       const NSPoint position   = [event locationInWindow];
       const NSRect contentRect = [self frame];
       m_pWindow->OnMouseMove(position.x, contentRect.size.height - position.y);
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)rightMouseDown:(NSEvent*) event
   {
       uint32 flags     = [event modifierFlags];
       uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
       m_pWindow->OnMouseButtonDown(Lambda::EMouseButton::MOUSEBUTTON_RIGHT, modifiers);
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)rightMouseDragged:(NSEvent*) event
   {
       [self mouseMoved:event];
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)rightMouseUp:(NSEvent*) event
   {
       uint32 flags     = [event modifierFlags];
       uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
       m_pWindow->OnMouseButtonUp(Lambda::EMouseButton::MOUSEBUTTON_RIGHT, modifiers);
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)otherMouseDown:(NSEvent*) event
   {
       Lambda::EMouseButton eButton = Lambda::MacPlatform::ConvertMouseButton((uint32)[event buttonNumber]);
       if (eButton != Lambda::EMouseButton::MOUSEBUTTON_UNKNOWN)
       {
           uint32 flags     = [event modifierFlags];
           uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
           m_pWindow->OnMouseButtonDown(eButton, modifiers);
       }
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)otherMouseDragged:(NSEvent*) event
   {
       [self mouseMoved:event];
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)otherMouseUp:(NSEvent*) event
   {
       Lambda::EMouseButton eButton = Lambda::MacPlatform::ConvertMouseButton((uint32)[event buttonNumber]);
       if (eButton != Lambda::EMouseButton::MOUSEBUTTON_UNKNOWN)
       {
           uint32 flags     = [event modifierFlags];
           uint32 modifiers = Lambda::MacPlatform::ConvertModifierKeyFlags(flags);
           m_pWindow->OnMouseButtonDown(eButton, modifiers);
       }
   }

   /*////////////////////////////////////////////////////////////////////////////////////////////////*/
   - (void)scrollWheel:(NSEvent*) event
   {
       double verticalValue    = [event scrollingDeltaY];
       double hortizontalValue = [event scrollingDeltaX];

       if ([event hasPreciseScrollingDeltas])
       {
           hortizontalValue *= 0.1;
           verticalValue *= 0.1;
       }

       if (fabs(hortizontalValue) > 0.0 || fabs(verticalValue) > 0.0)
       {
           m_pWindow->OnMouseScroll(float(hortizontalValue), float(verticalValue));
       }
   }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)insertText:(id) string replacementRange:(NSRange) replacementRange
    {
        //Get characters
        NSString* characters;
        if ([string isKindOfClass:[NSAttributedString class]])
        {
            characters = [string string];
        }
        else
        {
            characters = (NSString*) string;
        }

        //Send events
        const NSUInteger length = [characters length];
        for (NSUInteger i = 0;  i < length;  i++)
        {
            const uint32 codepoint = [characters characterAtIndex:i];
            if ((codepoint & 0xff00) != 0xf700)
            {
                m_pWindow->OnKeyTyped(codepoint);
            }
        }
    }
@end

//------------
//CCocoaWindow
//------------

@implementation CCocoaWindow

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (id) initWithContentRect:(Lambda::CMacWindow*) pWindow :(NSRect) contentRect styleMask:(NSWindowStyleMask) style backing:(NSBackingStoreType) backingStoreType defer:(BOOL) flag
    {
        //Call super and init window pointer
        self = [super initWithContentRect:contentRect styleMask:style backing:backingStoreType defer:flag];
        if (self)
        {
            m_pWindow = pWindow;
            [self setDelegate:self];
        }
        
        return self;
    }

    - (void)dealloc
    {
        m_pWindow = nullptr;
        [super dealloc];
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)canBecomeKeyWindow
    {
        return YES;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)canBecomeMainWindow
    {
        return YES;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)acceptsMouseMovedEvents
    {
        return YES;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)acceptsFirstResponder
    {
        return YES;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (BOOL)windowShouldClose:(NSNotification*) notification
    {
        m_pWindow->OnClose();
        return YES;
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidResize:(NSNotification*) notification
    {
        const NSRect contentRect = [[self contentView] frame];
        m_pWindow->OnResize(uint32(contentRect.size.width), uint32(contentRect.size.height));
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidMove:(NSNotification*) notification
    {
        const NSRect contentRect = [self contentRectForFrameRect:[self frame]];
        m_pWindow->OnMove(contentRect.origin.x, contentRect.origin.y);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidMiniaturize:(NSNotification*) notification
    {
        const NSRect contentRect = [[self contentView] frame];
        m_pWindow->OnMiniaturized(contentRect.size.width, contentRect.size.height, true);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidDeminiaturize:(NSNotification*) notification
    {
        const NSRect contentRect = [[self contentView] frame];
        m_pWindow->OnMiniaturized(contentRect.size.width, contentRect.size.height, false);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidEnterFullScreen:(NSNotification*) notification
    {
        m_pWindow->OnToggleFullscreen(true);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidExitFullScreen:(NSNotification*) notification
    {
        m_pWindow->OnToggleFullscreen(false);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidBecomeKey:(NSNotification*) notification
    {
        m_pWindow->OnFocusChanged(true);
    }

    /*////////////////////////////////////////////////////////////////////////////////////////////////*/
    - (void)windowDidResignKey:(NSNotification*) notification
    {
        m_pWindow->OnFocusChanged(false);
    }
@end

#endif
