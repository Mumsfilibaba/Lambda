#pragma once
#include "Event.h"

namespace Lambda
{
    //-----------------
    //WindowResizeEvent
    //-----------------

    class WindowResizeEvent : public Event
    {
    public:
		_forceinline WindowResizeEvent(uint32 width, uint32 height)
			: Event(EVENT_CATEGORY_WINDOW), m_Width(width), m_Height(height) {}
        ~WindowResizeEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(WindowResizeEvent);
        
		_forceinline uint32 GetWidth() const  
		{ 
			return m_Width; 
		}
		
		
		_forceinline uint32 GetHeight() const 
		{ 
			return m_Height; 
		}
    private:
        uint32 m_Width;
        uint32 m_Height;
    };

    //---------------
    //WindowMoveEvent
    //---------------

    class WindowMoveEvent : public Event
    {
    public:
		_forceinline WindowMoveEvent(uint32 x, uint32 y)
			: Event(EVENT_CATEGORY_WINDOW), m_X(x), m_Y(y) {}
        ~WindowMoveEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(WindowMoveEvent);
		
		_forceinline uint32 GetWidth() const  
		{ 
			return m_X; 
		}
		
		
		_forceinline uint32 GetHeight() const 
		{ 
			return m_Y; 
		}
    private:
        uint32 m_X;
        uint32 m_Y;
    };

    //-----------------------
    //WindowFocusChangedEvent
    //-----------------------

    class WindowFocusChangedEvent : public Event
    {
    public:
		_forceinline WindowFocusChangedEvent(bool hasFocus)
			: Event(EVENT_CATEGORY_WINDOW), m_HasFocus(hasFocus) {}
        ~WindowFocusChangedEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(WindowFocusChangedEvent);
		
		_forceinline bool HasFocus() const  
		{ 
			return m_HasFocus; 
		}
    private:
        bool m_HasFocus;
    };

    //-----------------
    //WindowClosedEvent
    //-----------------

    class WindowClosedEvent : public Event
    {
    public:
		_forceinline WindowClosedEvent()
			: Event(EVENT_CATEGORY_WINDOW) {}
        ~WindowClosedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(WindowClosedEvent);
    };
}
