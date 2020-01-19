#pragma once
#include "LambdaCore.h"

#include "Core/Input/EMouse.h"
#include "Core/Input/EKeyboard.h"

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

		SYSTEM_EVENT_APP_EXIT			= 8,
        SYSTEM_EVENT_APP_FOCUS_CHANGED	= 9,

		SYSTEM_EVENT_DISPLAY_RESIZED = 10,

		SYSTEM_EVENT_WINDOW_MOVE = 11,
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
	//SMouseMovedEvent
	//----------------

	struct SMouseMovedEvent
	{
		int32 x;
		int32 y;
	};

	//-----------------
	//SMouseButtonEvent
	//-----------------

	struct SMouseButtonEvent
	{
		EMouseButton Button;
		uint32 Modifiers;
		int32 x;
		int32 y;
	};

	//-------------------
	//SMouseScrolledEvent
	//-------------------

	struct SMouseScrolledEvent
	{
		float Horizontal;
		float Vertical;
	};

	//-----------
	//EResizeType
	//-----------

	enum class EResizeType : uint32
	{
		RESIZE_TYPE_UNKNOWN		= 0,
		RESIZE_TYPE_MAXIMIZED	= 1,
		RESIZE_TYPE_MINIMIZED	= 2,
		RESIZE_TYPE_STANDARD	= 3
	};

	//-------------------
	//SDisplayResizeEvent
	//-------------------

	struct SDisplayResizeEvent
	{
		EResizeType ResizeType;
		uint32 Width;
		uint32 Height;
	};

	//----------------
	//SWindowMoveEvent
	//----------------

	struct SWindowMoveEvent
	{
		uint32 x;
		uint32 y;
	};

	//---------------------
	//SAppFocusChangedEvent
	//---------------------

	struct SAppFocusChangedEvent
	{
		bool bHasFocus;
	};

	//-------------
	//SAppExitEvent
	//-------------

	struct SAppExitEvent
	{
		int32 ExitCode;
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

			SMouseMovedEvent MouseMovedEvent;
			SMouseScrolledEvent MouseScrolledEvent;
			SMouseButtonEvent MouseButtonEvent;

			SDisplayResizeEvent DisplayResizeEvent;
			
			SAppExitEvent AppExitEvent;
			SAppFocusChangedEvent AppFocusChangedEvent;
			
			SWindowMoveEvent WindowMovedEvent;
		};
    };
}
