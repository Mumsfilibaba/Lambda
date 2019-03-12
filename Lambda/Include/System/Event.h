#pragma once
#include "../Defines.h"
#include "../Types.h"

namespace Lambda
{
	enum EventType : uint32
	{
		EVENT_TYPE_UNKNOWN,
		EVENT_TYPE_WINDOW_CLOSED,
		EVENT_TYPE_WINDOW_RESIZE,
		EVENT_TYPE_WINDOW_MOVE,
		EVENT_TYPE_KEY,
		EVENT_TYPE_TEXT,
		EVENT_TYPE_MOUSE_MOVED,
		EVENT_TYPE_FOCUS_CHANGED
	};

	struct Event
	{
		EventType Type;
		union
		{
			struct
			{
				Key		KeyCode;
				bool	Down;
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
				uint16	PosX;
				uint16	PosY;
			} WindowMove;

			struct
			{
				uint16	Width;
				uint16	Height;
			} WindowResize;
		};
	};
}