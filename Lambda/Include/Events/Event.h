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
		EVENT_TYPE_UNKNOWN				    = 0,
		EVENT_TYPE_WINDOW_CLOSED		    = 1,
		EVENT_TYPE_WINDOW_RESIZE		    = 2,
		EVENT_TYPE_WINDOW_MOVE			    = 3,
		EVENT_TYPE_WINDOW_FOCUS_CHANGED	    = 4,
		EVENT_TYPE_KEY_PRESSED			    = 5,
		EVENT_TYPE_KEY_RELEASED			    = 6,
		EVENT_TYPE_KEY_TYPED			    = 7,
		EVENT_TYPE_MOUSE_BUTTON_PRESSED		= 8,
		EVENT_TYPE_MOUSE_BUTTON_RELEASED    = 9,
		EVENT_TYPE_MOUSE_MOVED			    = 10,
		EVENT_TYPE_MOUSE_SCROLLED		    = 11,
		EVENT_TYPE_JOYSTICK_CHANGED		    = 12
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
        EVENT_CATEGORY_INPUT        = (1 << 3),
		EVENT_CATEGORY_APPLICATION	= (1 << 4),
		EVENT_CATEGORY_OTHER		= (1 << 5)
	};

    //-----
    //Event
    //-----

    class Event
    {
    public:
        inline Event(EventType type, uint32 categories)
            : m_Type(type), m_Categories(categories) {}
        ~Event() = default;
        
        inline bool        IsHandled() const                    { return m_Handled; }
        inline bool        IsInCategory(EventCategory category) { return m_Categories & category; }
        inline EventType   GetType() const                      { return m_Type; }
        inline uint32      GetCategoryFlags() const             { return m_Categories; }
        
    private:
        bool        m_Handled;
        EventType   m_Type;
        uint32      m_Categories;
    };
    

	/*struct Event
	{
		EventType	Type            = EVENT_TYPE_UNKNOWN;
		uint32 	    CategoryFlags   = 0;
        bool        IsHandled       = false;
		union
		{
			struct
			{
				Key		KeyCode;
                uint32  ModiferKeys;
				uint32	RepeatCount;
			} KeyEvent;

			struct
			{
				uint32	Character;
			} KeyTypedEvent;

			struct
			{
				int32	PosX;
				int32	PosY;
			} MouseMoveEvent;

			struct
			{
				MouseButton	Button;
                uint32      ModiferKeys;
			} MouseButtonEvent;

			struct
			{
				float   VerticalValue;
				float   HorizontalValue;
			} MouseScrollEvent;

			struct
			{
				uint32	PosX;
				uint32	PosY;
			} WindowMove;

			struct
			{
				uint32	Width;
				uint32	Height;
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
	};*/
}
