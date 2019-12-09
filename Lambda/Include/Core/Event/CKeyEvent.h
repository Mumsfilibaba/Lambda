#pragma once
#include "CEvent.h"
#include "../Input/EKey.h"

namespace Lambda
{
    //---------
    //CKeyEvent
    //---------

    class CKeyEvent : public CEvent
    {
    public:
		_forceinline CKeyEvent(EKey key, uint32 modifers)
            : CEvent(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT), m_Key(key), m_Modifiers(modifers) {}
        virtual ~CKeyEvent() = default;
        
		_forceinline bool HasModifer(EKeyModifier modifier) const
		{ 
			return m_Modifiers & modifier; 
		}
		
		
		_forceinline EKey GetKey() const
		{ 
			return m_Key; 
		}
		
		
		_forceinline uint32  GetModifiers() const                     
		{ 
			return m_Modifiers; 
		}
    private:
        EKey m_Key;
        uint32 m_Modifiers;
    };

    //-----------------
    //CKeyReleasedEvent
    //-----------------

    class CKeyReleasedEvent : public CKeyEvent
    {
    public:
		_forceinline CKeyReleasedEvent(Key key, uint32 modifiers)
            : CKeyEvent(key, modifiers) {}
        ~CKeyReleasedEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(CKeyReleasedEvent);
    };

    //----------------
    //CKeyPressedEvent
    //----------------
  
    class CKeyPressedEvent : public CKeyEvent
    {
    public:
		_forceinline CKeyPressedEvent(Key key, uint32 modifiers, uint32 repeatCount)
            : CKeyEvent(key, modifiers), m_RepeatCount(repeatCount) {}
        ~CKeyPressedEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(CKeyPressedEvent);
		
		_forceinline uint32 GetRepeatCount() const  
		{ 
			return m_RepeatCount; 
		}
    private:
        uint32  m_RepeatCount;
    };

    //--------------
    //CKeyTypedEvent
    //--------------

    class CKeyTypedEvent : public CEvent
    {
    public:
		_forceinline CKeyTypedEvent(uint32 character)
			: CEvent(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT), m_Character(character) {}
		~CKeyTypedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(CKeyTypedEvent);
		
		_forceinline uint32 GetCharacter() const  
		{ 
			return m_Character; 
		}
    private:
        uint32 m_Character;
    };
}
