#pragma once
#include "Defines.h"
#include "Types.h"
#include "Utilities/HashHelper.h"

#define LAMBDA_DECLARE_EVENT_TYPE(x)	_forceinline static constexpr Lambda::EventType GetStaticType()	{ constexpr Lambda::EventType hash = Lambda::StringHash(#x); return hash; } \
										virtual Lambda::EventType GetType() const override final	{ return x::GetStaticType(); }

namespace Lambda
{
    //---------
    //EventType
    //---------
    
	typedef size_t EventType;

	//-------------
	//EventCategory
	//-------------

	enum EventCategory : uint32
	{
		EVENT_CATEGORY_UNKNOWN		= 0,
		EVENT_CATEGORY_WINDOW		= (1 << 0),
		EVENT_CATEGORY_KEYBOARD		= (1 << 1),
		EVENT_CATEGORY_MOUSE		= (1 << 2),
		EVENT_CATEGORY_JOYSTICK		= (1 << 3),
        EVENT_CATEGORY_INPUT        = (1 << 3),
		EVENT_CATEGORY_APPLICATION	= (1 << 4),
		EVENT_CATEGORY_OTHER		= (1 << 5),
		EVENT_CATEGORY_ALL			= 0xffffffff
	};

    //-----
    //Event
    //-----

    class Event
    {
        friend class Application;
        friend class EventForwarder;
        friend class EventDispatcher;
    public:
        _forceinline Event(uint32 categories)
            : m_Handled(false), m_Categories(categories) {}
        virtual ~Event() = default;
        
		virtual EventType GetType() const = 0;
		
		_forceinline bool IsHandled() const                    
		{ 
			return m_Handled; 
		}


		_forceinline bool IsInCategory(EventCategory category) 
		{ 
			return (m_Categories & category); 
		}
        
		
		_forceinline uint32 GetCategoryFlags() const           
		{ 
			return m_Categories; 
		}
	private:
		_forceinline void SetIsHandled(bool isHandled) const 
		{ 
			m_Handled = isHandled; 
		}
    private:
        mutable bool m_Handled;
        uint32 m_Categories;
	private:
		_forceinline static constexpr Lambda::EventType GetStaticType()
		{ 
			constexpr Lambda::EventType hash = Lambda::StringHash("Event");
			return hash; 
		}
    };
}
