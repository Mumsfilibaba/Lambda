#pragma once
#include "Defines.h"
#include "Types.h"
#include "Utilities/HashHelper.h"

#define LAMBDA_DECLARE_EVENT_TYPE(x)	inline static constexpr Lambda::EventType GetStaticType()	{ constexpr Lambda::EventType hash = Lambda::StringHash(#x); return hash; } \
										virtual Lambda::EventType GetType() const override final	{ return x::GetStaticType(); }

namespace Lambda
{
    //---------
    //EventType
    //---------
    
	typedef size_t EventType;

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
		EVENT_CATEGORY_OTHER		= (1 << 5),
		EVENT_CATEGORY_ALL			= 0xffffffff
	};

    //-----
    //Event
    //-----

    class Event
    {
		friend class EventDispatcher;

    public:
        inline Event(uint32 categories)
            : m_Handled(false), m_Categories(categories) {}
        virtual ~Event() = default;
        
        inline bool        IsHandled() const                    { return m_Handled; }
        inline bool        IsInCategory(EventCategory category) { return m_Categories & category; }
        inline uint32      GetCategoryFlags() const             { return m_Categories; }

		virtual EventType GetType() const = 0;
        
	private:
		inline void SetIsHandled(bool isHandled) const { m_Handled = isHandled; }
    private:
        mutable bool    m_Handled;
        uint32			m_Categories;

	private:
		inline static constexpr Lambda::EventType GetStaticType()
		{ 
			constexpr Lambda::EventType hash = Lambda::StringHash("Event");
			return hash; 
		}
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
