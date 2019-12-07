#pragma once
#include "LamdbaCore.h"

namespace Lambda
{
    //------------
    //ESystemEvent
    //------------

    enum ESystemEvent: uint32
    {
        SYSTEM_EVENT_UNKNOWN                = 0,
        SYSTEM_EVENT_WINDOW_MOVED           = 1,
        SYSTEM_EVENT_WINDOW_CLOSED          = 2,
        SYSTEM_EVENT_WINDOW_FOCUS_CHANGED   = 3,
        SYSTEM_EVENT_WINDOW_RESIZE          = 4,
        SYSTEM_EVENT_MOUSE_SCROLLED         = 5,
        SYSTEM_EVENT_MOUSE_BUTTON_UP        = 6,
        SYSTEM_EVENT_MOUSE_BUTTON_DOWN      = 7,
        SYSTEM_EVENT_MOUSE_MOVE             = 8,
        SYSTEM_EVENT_KEYBOARD_UP            = 9,
        SYSTEM_EVENT_KEYBOARD_DOWN          = 10,
        SYSTEM_EVENT_KEYBOARD_TEXT          = 11,
        SYSTEM_EVENT_QUIT                   = 12,
    };

    //----------------
    //ESystemEventType
    //----------------
    enum ESystemEventType : uint32
    {
        SYSTEM_EVENT_TYPE_UNKNOWN   = (0),
        SYSTEM_EVENT_TYPE_WINDOW    = (1 << 1),
        SYSTEM_EVENT_TYPE_KEYBOARD  = (1 << 2),
        SYSTEM_EVENT_TYPE_MOUSE     = (1 << 3),
        SYSTEM_EVENT_TYPE_SYSTEM    = (1 << 4),
    };

    //------------
    //SSystemEvent
    //------------

    struct SSystemEvent
    {
        ESystemEvent Event = SYSTEM_EVENT_UNKNOWN;
        ESystemEventType EventType = SYSTEM_EVENT_TYPE_UNKNOWN;
        
        struct SWindowResize
        {
            uint32 Width;
            uint32 Height;
        };
        
        struct SWindowFocus
        {
            bool HasFocus;
        };
        
        struct SWindowMove
        {
            uint32 PositionX;
            uint32 PositionY;
        };
        
        struct SMouseButton
        {
            //TODO: Insert buttoncode here
        };
        
        struct SMouseScrolled
        {
            float HorizontalScroll;
            float VerticalScoll;
        };
        
        struct SMouseMove
        {
            uint32 PositionX;
            uint32 PositionY;
        };
        
        struct SKeyboard
        {
            // TODO: Insert keycode here
        };
        
        struct SKeyboardText
        {
            uint32 Character;
        };
        
        union
        {
            SWindowResize WindowResize;
            SWindowFocus WindowFocus;
            SWindowMove WindowMove;
            SMouseButton MouseButton;
            SMouseScrolled MouseScrolled;
            SMouseMove MouseMove;
            SKeyboard Keyboard;
            SKeyboardText KeyboardText;
        };
    };
}
