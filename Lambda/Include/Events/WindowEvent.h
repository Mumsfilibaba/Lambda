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
        inline WindowResizeEvent(uint32 width, uint32 height)
        : Event(EVENT_CATEGORY_WINDOW), m_Width(width), m_Height(height) {}
        ~WindowResizeEvent() = default;
        
        inline uint32 GetWidth() const  { return m_Width; }
        inline uint32 GetHeight() const { return m_Height; }
		LAMBDA_DECLARE_EVENT_TYPE(WindowResizeEvent);
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
        inline WindowMoveEvent(uint32 x, uint32 y)
        : Event(EVENT_CATEGORY_WINDOW), m_X(x), m_Y(y) {}
        ~WindowMoveEvent() = default;
        
        inline uint32 GetWidth() const  { return m_X; }
        inline uint32 GetHeight() const { return m_Y; }
		LAMBDA_DECLARE_EVENT_TYPE(WindowMoveEvent);
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
        inline WindowFocusChangedEvent(bool hasFocus)
        : Event(EVENT_CATEGORY_WINDOW), m_HasFocus(hasFocus) {}
        ~WindowFocusChangedEvent() = default;
        
        inline bool HasFocus() const  { return m_HasFocus; }
		LAMBDA_DECLARE_EVENT_TYPE(WindowFocusChangedEvent);
    private:
        bool m_HasFocus;
    };

    //-----------------
    //WindowClosedEvent
    //-----------------

    class WindowClosedEvent : public Event
    {
    public:
        inline WindowClosedEvent()
        : Event(EVENT_CATEGORY_WINDOW) {}
        ~WindowClosedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(WindowClosedEvent);
    };
}
