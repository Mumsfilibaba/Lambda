#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
	enum EventType : uint32
	{
		EVENT_TYPE_UNKNOWN = 0,
		EVENT_TYPE_WINDOW_CLOSED,
		EVENT_TYPE_WINDOW_RESIZE,
		EVENT_TYPE_WINDOW_MOVE,
		EVENT_TYPE_KEYDOWN,
		EVENT_TYPE_KEYUP,
		EVENT_TYPE_TEXT,
		EVENT_TYPE_MOUSE_BUTTONDOWN,
		EVENT_TYPE_MOUSE_BUTTONUP,
		EVENT_TYPE_MOUSE_MOVED,
		EVENT_TYPE_MOUSE_SCROLLED,
		EVENT_TYPE_FOCUS_CHANGED,
		EVENT_TYPE_JOYSTICK_CHANGED
	};

	struct Event
	{
		EventType Type;
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
				uint16	PosX;
				uint16	PosY;
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
				uint16	PosX;
				uint16	PosY;
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