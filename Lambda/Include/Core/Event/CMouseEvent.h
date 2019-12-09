#pragma once
#include "CEvent.h"
#include "../Input/EKey.h"
#include "../Input/EMouseButton.h"

namespace Lambda
{
    //-----------------
    //CMouseButtonEvent
    //-----------------

    class CMouseButtonEvent : public CEvent
    {
    public:
        _forceinline CMouseButtonEvent(EMouseButton button, uint32 modifiers)
			: CEvent(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_Button(button), m_Modifiers(modifiers) {}
        ~CMouseButtonEvent() = default;
        
		_forceinline bool HasModifierKey(EKeyModifier modifier)
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
        EMouseButton m_Button;
        uint32 m_Modifiers;
    };

    //------------------------
    //CMouseButtonPressedEvent
    //------------------------

    class CMouseButtonPressedEvent : public CMouseButtonEvent
    {
    public:
		_forceinline CMouseButtonPressedEvent(EMouseButton button, uint32 modifiers)
			: CMouseButtonEvent(button, modifiers) {}
        ~CMouseButtonPressedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(CMouseButtonPressedEvent);
    };

    //-------------------------
    //CMouseButtonReleasedEvent
    //-------------------------

    class CMouseButtonReleasedEvent : public CMouseButtonEvent
    {
    public:
		_forceinline  CMouseButtonReleasedEvent(EMouseButton button, uint32 modifiers)
			: CMouseButtonEvent(button, modifiers) {}
        ~CMouseButtonReleasedEvent() = default;

		LAMBDA_DECLARE_EVENT_TYPE(CMouseButtonReleasedEvent);
    };

    //----------------
    //CMouseMovedEvent
    //----------------

    class CMouseMovedEvent : public CEvent
    {
    public:
		_forceinline  CMouseMovedEvent(int32 x, int32 y)
			: CEvent(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_X(x), m_Y(y) {}
        ~CMouseMovedEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(CMouseMovedEvent);

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

    //-------------------
    //CMouseScrolledEvent
    //-------------------

    class CMouseScrolledEvent : public CEvent
    {
    public:
		_forceinline CMouseScrolledEvent(float horizontal, float vertical)
        : CEvent(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT), m_Horizontal(horizontal), m_Vertical(vertical)  {}
        ~CMouseScrolledEvent() = default;
        
		LAMBDA_DECLARE_EVENT_TYPE(CMouseScrolledEvent);
		
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
