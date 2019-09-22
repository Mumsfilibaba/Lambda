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
        inline MouseButtonEvent(EventType type, MouseButton button, uint32 modifiers)
        : Event(type, EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_Button(button) {}
        ~MouseButtonEvent() = default;
        
        inline bool         HasModifierKey(KeyModifier modifier)    { return m_Modifiers & modifier; }
        inline MouseButton  GetButton() const                       { return m_Button; }
        inline uint32       GetModifiers() const                    { return m_Modifiers; }
        
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
        inline MouseButtonPressedEvent(MouseButton button, uint32 modifiers)
        : MouseButtonEvent(EVENT_TYPE_MOUSE_BUTTON_PRESSED, button, modifiers) {}
        ~MouseButtonPressedEvent() = default;
    };

    //------------------------
    //MouseButtonReleasedEvent
    //------------------------

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        inline MouseButtonReleasedEvent(MouseButton button, uint32 modifiers)
        : MouseButtonEvent(EVENT_TYPE_MOUSE_BUTTON_RELEASED, button, modifiers) {}
        ~MouseButtonReleasedEvent() = default;
    };

    //---------------
    //MouseMovedEvent
    //---------------

    class MouseMovedEvent : public Event
    {
    public:
        inline MouseMovedEvent(int32 x, int32 y)
        : Event(EVENT_TYPE_MOUSE_MOVED, EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_X(x), m_Y(y) {}
        ~MouseMovedEvent() = default;
        
        inline int32 GetX() const { return m_X; }
        inline int32 GetY() const { return m_Y; }
        
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
        inline MouseScrolledEvent(float x, float y)
        : Event(EVENT_TYPE_MOUSE_SCROLLED, EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_X(x), m_Y(y)  {}
        ~MouseScrolledEvent() = default;
        
        inline int32 GetHorizontalValue() const { return m_X; }
        inline int32 GetVerticalValue() const   { return m_Y; }
        
    private:
        float m_X;
        float m_Y;
    };
}
