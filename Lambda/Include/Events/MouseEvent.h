#pragma once
#include "Event.h"

namespace Lambda
{
    //----------------
    //MouseButtonEvent
    //----------------

    class MouseButtonEvent : public Event
    {
    public:
        _forceinline MouseButtonEvent(MouseButton button, uint32 modifiers)
			: Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_Button(button), m_Modifiers(modifiers) {}
        ~MouseButtonEvent() = default;
        
		_forceinline bool HasModifierKey(KeyModifier modifier)    
		{ 
			return m_Modifiers & modifier; 
		}
		
		
		_forceinline MouseButton GetButton() const                       
		{ 
			return m_Button; 
		}
		
		
		_forceinline uint32 GetModifiers() const                    
		{ 
			return m_Modifiers; 
		}
    private:
        MouseButton m_Button;
        uint32      m_Modifiers;
    };

    //-----------------------
    //MouseButtonPressedEvent
    //-----------------------

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
		_forceinline  MouseButtonPressedEvent(MouseButton button, uint32 modifiers)
			: MouseButtonEvent(button, modifiers) {}
        ~MouseButtonPressedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(MouseButtonPressedEvent);
    };

    //------------------------
    //MouseButtonReleasedEvent
    //------------------------

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
		_forceinline  MouseButtonReleasedEvent(MouseButton button, uint32 modifiers)
			: MouseButtonEvent(button, modifiers) {}
        ~MouseButtonReleasedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(MouseButtonReleasedEvent);
    };

    //---------------
    //MouseMovedEvent
    //---------------

    class MouseMovedEvent : public Event
    {
    public:
		_forceinline  MouseMovedEvent(int32 x, int32 y)
			: Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_X(x), m_Y(y) {}
        ~MouseMovedEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(MouseMovedEvent);

		_forceinline int32 GetX() const 
		{ 
			return m_X; 
		}


		_forceinline int32 GetY() const 
		{ 
			return m_Y; 
		}
    private:
        int32 m_X;
        int32 m_Y;
    };

    //------------------
    //MouseScrolledEvent
    //------------------

    class MouseScrolledEvent : public Event
    {
    public:
		_forceinline MouseScrolledEvent(float horizontal, float vertical)
        : Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_Horizontal(horizontal), m_Vertical(vertical)  {}
        ~MouseScrolledEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(MouseScrolledEvent);
		
		_forceinline float GetHorizontalValue() const 
		{ 
			return m_Horizontal; 
		}
		
		
		_forceinline float GetVerticalValue() const   
		{ 
			return m_Vertical; 
		}
    private:
        float m_Horizontal;
        float m_Vertical;
    };
}
