#pragma once
#include "CEvent.h"

namespace Lambda
{
    //------------------
    //CWindowResizeEvent
    //------------------

    class CWindowResizeEvent : public CEvent
    {
    public:
		_forceinline CWindowResizeEvent(uint32 width, uint32 height)
			: CEvent(EVENT_CATEGORY_WINDOW), m_Width(width), m_Height(height) {}
        ~CWindowResizeEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(CWindowResizeEvent);
        
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

    //----------------
    //CWindowMoveEvent
    //----------------

    class CWindowMoveEvent : public CEvent
    {
    public:
		_forceinline CWindowMoveEvent(uint32 x, uint32 y)
			: CEvent(EVENT_CATEGORY_WINDOW), m_X(x), m_Y(y) {}
        ~CWindowMoveEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(CWindowMoveEvent);
		
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

    //------------------------
    //CWindowFocusChangedEvent
    //------------------------

    class CWindowFocusChangedEvent : public CEvent
    {
    public:
		_forceinline CWindowFocusChangedEvent(bool hasFocus)
			: CEvent(EVENT_CATEGORY_WINDOW), m_HasFocus(hasFocus) {}
        ~CWindowFocusChangedEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(CWindowFocusChangedEvent);
		
		_forceinline bool HasFocus() const  
		{ 
			return m_HasFocus; 
		}
    private:
        bool m_HasFocus;
    };

    //------------------
    //CWindowClosedEvent
    //------------------

    class CWindowClosedEvent : public CEvent
    {
    public:
		_forceinline CWindowClosedEvent()
			: CEvent(EVENT_CATEGORY_WINDOW) {}
        ~CWindowClosedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(CWindowClosedEvent);
    };
}
