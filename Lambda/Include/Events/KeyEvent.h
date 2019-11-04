#pragma once
#include "Event.h"

namespace Lambda
{
    //--------
    //KeyEvent
    //--------

    class KeyEvent : public Event
    {
    public:
		_forceinline KeyEvent(Key key, uint32 modifers)
            : Event(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT), m_Key(key), m_Modifiers(modifers) {}
        virtual ~KeyEvent() = default;
        
		_forceinline bool HasModifer(KeyModifier modifier) const   
		{ 
			return m_Modifiers & modifier; 
		}
		
		
		_forceinline Key GetKey() const
		{ 
			return m_Key; 
		}
		
		
		_forceinline uint32  GetModifiers() const                     
		{ 
			return m_Modifiers; 
		}
    private:
        Key m_Key;
        uint32 m_Modifiers;
    };

    //----------------
    //KeyReleasedEvent
    //----------------

    class KeyReleasedEvent : public KeyEvent
    {
    public:
		_forceinline KeyReleasedEvent(Key key, uint32 modifiers)
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
		_forceinline KeyPressedEvent(Key key, uint32 modifiers, uint32 repeatCount)
            : KeyEvent(key, modifiers), m_RepeatCount(repeatCount) {}
        ~KeyPressedEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(KeyPressedEvent);
		
		_forceinline uint32 GetRepeatCount() const  
		{ 
			return m_RepeatCount; 
		}
    private:
        uint32  m_RepeatCount;
    };

    //-------------
    //KeyTypedEvent
    //-------------

    class KeyTypedEvent : public Event
    {
    public:
		_forceinline KeyTypedEvent(uint32 character)
			: Event(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT), m_Character(character) {}
		~KeyTypedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(KeyTypedEvent);
		
		_forceinline uint32 GetCharacter() const  
		{ 
			return m_Character; 
		}
    private:
        uint32 m_Character;
    };
}
