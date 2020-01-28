#pragma once
#include "Core/Input/Mouse.h"
#include "Core/Input/Keyboard.h"

namespace Lambda
{
	class IWindow;

	enum class EResizeType : uint8
	{
		RESIZE_TYPE_STANDARD = 1,
		RESIZE_TYPE_MINIMIZE = 2,
		RESIZE_TYPE_MAXIMIZE = 3,
	};

	enum class EEventType : uint8
	{
		EVENT_TYPE_MOUSE_BUTTON_PRESSED		= 1,
		EVENT_TYPE_MOUSE_BUTTON_RELEASED	= 2,
		EVENT_TYPE_MOUSE_MOVED				= 3,
		EVENT_TYPE_MOUSE_SCROLLED			= 4,

		EVENT_TYPE_KEY_PRESSED	= 5,
		EVENT_TYPE_KEY_RELEASED = 6,
		EVENT_TYPE_KEY_TYPED	= 7,

		EVENT_TYPE_WINDOW_RESIZE	= 8,
		EVENT_TYPE_WINDOW_MOVE		= 9,
		EVENT_TYPE_WINDOW_CLOSED	= 10,
		EVENT_TYPE_WINDOW_FOCUS		= 11
	};

	struct SMouseButtonEvent
	{
		EMouseButton Button;
		uint32 Modifiers;
		int32 x;
		int32 y;
	};

	struct SMouseMoveEvent
	{
		int32 x;
		int32 y;
	};

	struct SMouseScrolledEvent
	{
		float Horizontal;
		float Vertical;
	};

	struct SKeyEvent
	{
		EKey Key;
		uint32 RepeatCount;
		uint32 Modifiers;
	};

	struct SKeyTypedEvent
	{
		uint32 Character;
	};

	struct SWindowResizeEvent
	{
		EResizeType ResizeType;
		uint32 Width;
		uint32 Height;
	};

	struct SWindowMoveEvent
	{
		uint32 x;
		uint32 y;
	};

	struct SWindowFocusEvent
	{
		bool bHasFocus;
	};

	struct SEvent
	{
		EEventType EventType;

		union
		{
			SKeyEvent KeyEvent;
			SKeyTypedEvent KeyTypedEvent;

			SMouseButtonEvent MouseButtonEvent;
			SMouseMoveEvent MouseMoveEvent;
			SMouseScrolledEvent MouseScrolledEvent;

			SWindowResizeEvent WindowResizeEvent;
			SWindowMoveEvent WindowMoveEvent;
			SWindowFocusEvent WindowFocusEvent;
		};
	};
}
