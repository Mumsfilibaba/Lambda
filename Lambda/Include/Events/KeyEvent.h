#pragma once
#include "Event.h"
#include "Types.h"

namespace Lambda
{
    //--------
    //KeyEvent
    //--------

    class KeyEvent : public Event
    {
    public:
        inline KeyEvent(Key key, uint32 modifers)
            : Event(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT), m_Key(key), m_Modifiers(modifers) {}
        virtual ~KeyEvent() = default;
        
        inline bool    HasModifer(KeyModifier modifier) const   { return m_Modifiers & modifier; }
        inline Key     GetKey() const                           { return m_Key; }
        inline uint32  GetModifiers() const                     { return m_Modifiers; }

    private:
        Key     m_Key;
        uint32  m_Modifiers;
    };

    //----------------
    //KeyReleasedEvent
    //----------------

    class KeyReleasedEvent : public KeyEvent
    {
    public:
        inline KeyReleasedEvent(Key key, uint32 modifiers)
            : KeyEvent(key, modifiers) {}
        ~KeyReleasedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(KeyReleasedEvent);
    };

    //---------------
    //KeyPressedEvent
    //---------------
  
    class KeyPressedEvent : public KeyEvent
    {
    public:
        inline KeyPressedEvent(Key key, uint32 modifiers, uint32 repeatCount)
            : KeyEvent(key, modifiers), m_RepeatCount(repeatCount) {}
        ~KeyPressedEvent() = default;
        
        inline uint32 GetRepeatCount() const  { return m_RepeatCount; }
        
		LAMBDA_DECLARE_EVENT_TYPE(KeyPressedEvent);

    private:
        uint32  m_RepeatCount;
        
    };

    //-------------
    //KeyTypedEvent
    //-------------

    class KeyTypedEvent : public Event
    {
    public:
        inline KeyTypedEvent(uint32 character)
        : Event(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT), m_Character(character) {}
        
        inline uint32 GetCharacter() const  { return m_Character; }
        
		LAMBDA_DECLARE_EVENT_TYPE(KeyTypedEvent);

    private:
        uint32 m_Character;
    };
}
