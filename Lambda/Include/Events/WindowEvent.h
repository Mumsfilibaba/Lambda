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
        : Event(EVENT_TYPE_WINDOW_RESIZE, EVENT_CATEGORY_WINDOW), m_Width(width), m_Height(height) {}
        ~WindowResizeEvent() = default;
        
        inline uint32 GetWidth() const  { return m_Width; }
        inline uint32 GetHeight() const { return m_Height; }
        
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
        : Event(EVENT_TYPE_WINDOW_MOVE, EVENT_CATEGORY_WINDOW), m_X(x), m_Y(y) {}
        ~WindowMoveEvent() = default;
        
        inline uint32 GetWidth() const  { return m_X; }
        inline uint32 GetHeight() const { return m_Y; }
        
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
        : Event(EVENT_TYPE_WINDOW_FOCUS_CHANGED, EVENT_CATEGORY_WINDOW), m_HasFocus(hasFocus) {}
        ~WindowFocusChangedEvent() = default;
        
        inline bool HasFocus() const  { return m_HasFocus; }
        
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
        : Event(EVENT_TYPE_WINDOW_CLOSED, EVENT_CATEGORY_WINDOW) {}
        ~WindowClosedEvent() = default;
    };
}
