#pragma once
#include "LambdaCore.h"
#include "SWindowEvent.h"
#include "SKeyEvent.h"
#include "SMouseEvent.h"

namespace Lambda
{
	//------------
	//ESystemEvent
	//------------
	enum class ESystemEvent : uint32
	{
		SYSTEM_EVENT_UNKNOWN = 0,
		
		SYSTEM_EVENT_KEY_RELEASED = 1,
		SYSTEM_EVENT_KEY_PRESSED  = 2,
		SYSTEM_EVENT_KEY_TEXT	  = 3,

		SYSTEM_EVENT_MOUSE_MOVED		   = 4,
		SYSTEM_EVENT_MOUSE_SCROLLED		   = 5,
		SYSTEM_EVENT_MOUSE_BUTTON_RELEASED = 6,
		SYSTEM_EVENT_MOUSE_BUTTON_PRESSED  = 7,

        SYSTEM_EVENT_WINDOW_FOCUS_CHANGED = 8,
		SYSTEM_EVENT_WINDOW_RESIZED		  = 9,
		SYSTEM_EVENT_WINDOW_MOVED		  = 10,
	};

    //------
    //SEvent
    //------
    struct SEvent
    {
		ESystemEvent EventType = ESystemEvent::SYSTEM_EVENT_UNKNOWN;
		union
		{
			SKeyEvent KeyEvent;
			SKeyTextEvent KeyTextEvent;

			SMouseMovedEvent MouseMovedEvent;
			SMouseScrolledEvent MouseScrolledEvent;
			SMouseButtonEvent MouseButtonEvent;

			SWindowFocusChangedEvent WindowFocusChangedEvent;
			SWindowResizeEvent WindowResizedEvent;
			SWindowMoveEvent WindowMovedEvent;
		};
    };
}
