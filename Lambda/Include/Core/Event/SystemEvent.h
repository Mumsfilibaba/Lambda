#pragma once
#include "LambdaCore.h"

#include "Core/Input/EMouse.h"
#include "Core/Input/EKeyboard.h"

#include "WindowEvent.h"

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

	//---------
	//SKeyEvent 
	//---------

	struct SKeyEvent
	{
		EKey Key;
		uint32 RepeatCount;
		uint32 Modifiers;
	};

	//-------------
	//SKeyTextEvent
	//-------------

	struct SKeyTextEvent
	{
		uint32 Character;
	};

	//----------------
	//MouseButtonEvent
	//----------------

	struct MouseButtonEvent
	{
		EMouseButton Button;
		uint32 Modifiers;
	};

	//---------------
	//MouseMovedEvent
	//---------------

	struct MouseMovedEvent
	{
		int32 x;
		int32 y;
	};

	//------------------
	//MouseScrolledEvent
	//------------------

	struct MouseScrolledEvent
	{
		float Horizontal;
		float Vertical;
	};

    //------------
    //SSystemEvent
    //------------

    struct SSystemEvent
    {
		ESystemEvent EventType = ESystemEvent::SYSTEM_EVENT_UNKNOWN;
		union
		{
			SKeyEvent KeyEvent;
			SKeyTextEvent KeyTextEvent;

			MouseMovedEvent MouseMovedEvent;
			MouseScrolledEvent MouseScrolledEvent;
			MouseButtonEvent MouseButtonEvent;

			WindowFocusChangedEvent WindowFocusChangedEvent;
			WindowResizeEvent WindowResizedEvent;
			WindowMoveEvent WindowMovedEvent;
		};
    };
}
