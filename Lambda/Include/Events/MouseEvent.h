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
        inline MouseButtonEvent(MouseButton button, uint32 modifiers)
        : Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_Button(button), m_Modifiers(modifiers) {}
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
        inline MouseButtonReleasedEvent(MouseButton button, uint32 modifiers)
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
        inline MouseMovedEvent(int32 x, int32 y)
        : Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_X(x), m_Y(y) {}
        ~MouseMovedEvent() = default;
        
        inline int32 GetX() const { return m_X; }
        inline int32 GetY() const { return m_Y; }
		LAMBDA_DECLARE_EVENT_TYPE(MouseMovedEvent);
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
        inline MouseScrolledEvent(float horizontal, float vertical)
        : Event(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_Horizontal(horizontal), m_Vertical(vertical)  {}
        ~MouseScrolledEvent() = default;
        
        inline float GetHorizontalValue() const { return m_Horizontal; }
        inline float GetVerticalValue() const   { return m_Vertical; }
		LAMBDA_DECLARE_EVENT_TYPE(MouseScrolledEvent);
    private:
        float m_Horizontal;
        float m_Vertical;
    };
}
