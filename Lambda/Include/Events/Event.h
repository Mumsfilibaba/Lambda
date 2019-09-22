#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
    //---------
    //EventType
    //---------
    
	enum EventType : uint32
	{
		EVENT_TYPE_UNKNOWN				= 0,
		EVENT_TYPE_WINDOW_CLOSED		= 1,
		EVENT_TYPE_WINDOW_RESIZE		= 2,
		EVENT_TYPE_WINDOW_MOVE			= 3,
		EVENT_TYPE_WINDOW_FOCUS_CHANGED	= 4,
		EVENT_TYPE_KEYDOWN				= 5,
		EVENT_TYPE_KEYUP				= 6,
		EVENT_TYPE_KEYTYPED				= 7,
		EVENT_TYPE_MOUSE_BUTTONDOWN		= 8,
		EVENT_TYPE_MOUSE_BUTTONUP		= 9,
		EVENT_TYPE_MOUSE_MOVED			= 10,
		EVENT_TYPE_MOUSE_SCROLLED		= 11,
		EVENT_TYPE_JOYSTICK_CHANGED		= 12
	};

	//-------------
	//EventCategory
	//-------------

	enum EventCategory : uint32
	{
		EVENT_CATEGORY_UNKNOWN		= 0,
		EVENT_CATEGORY_WINDOW		= (1 << 0),
		EVENT_CATEGORY_KEYBOARD		= (1 << 1),
		EVENT_CATEGORY_MOUSE		= (1 << 2),
		EVENT_CATEGORY_JOYSTICK		= (1 << 3),
		EVENT_CATEGORY_APPLICATION	= (1 << 4),
		EVENT_CATEGORY_OTHER		= (1 << 5)
	};

    //-----
    //Event
    //-----

	struct Event
	{
		EventType		Type;
		EventCategory	Category;
		union
		{
			struct
			{
				Key		KeyCode;
				uint16	RepeatCount;
			} KeyEvent;

			struct
			{
				uint32	Character;
			} TextEvent;

			struct
			{
				int16	PosX;
				int16	PosY;
			} MouseMoveEvent;

			struct
			{
				MouseButton	Button;
			} MouseButtonEvent;

			struct
			{
				float Value;
				bool Vertical;
			} MouseScrollEvent;

			struct
			{
				int16	PosX;
				int16	PosY;
			} WindowMove;

			struct
			{
				uint16	Width;
				uint16	Height;
			} WindowResize;

			struct
			{
				bool HasFocus;
			} FocusChanged;

			struct
			{
				uint8 ControllerID;
				uint8 LeftTrigger;
				uint8 RightTrigger;
				int16 LeftX;
				int16 LeftY;
				int16 RightX;
				int16 RightY;
			} JoystickChanged;
		};
	};
}
