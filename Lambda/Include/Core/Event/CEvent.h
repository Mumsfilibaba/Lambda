#pragma once
#include "LambdaCore.h"
#include "Utilities/HashHelper.h"

#define LAMBDA_DECLARE_EVENT_TYPE(x)	_forceinline static constexpr Lambda::EventType GetStaticType()	{ constexpr Lambda::EventType hash = Lambda::StringHash(#x); return hash; } \
										virtual Lambda::EventType GetType() const override final	    { return x::GetStaticType(); }

namespace Lambda
{
    //---------
    //EventType
    //---------
    
	typedef size_t EventType;

	//--------------
	//EEventCategory
	//--------------

	enum EEventCategory : uint32
	{
		EVENT_CATEGORY_UNKNOWN		= 0,
		EVENT_CATEGORY_WINDOW		= (1 << 0),
		EVENT_CATEGORY_KEYBOARD		= (1 << 1),
		EVENT_CATEGORY_MOUSE		= (1 << 2),
		EVENT_CATEGORY_JOYSTICK		= (1 << 3),
        EVENT_CATEGORY_INPUT        = (1 << 3),
		EVENT_CATEGORY_ENVIRONMENT	= (1 << 4),
		EVENT_CATEGORY_OTHER		= (1 << 5),
		EVENT_CATEGORY_ALL			= 0xffffffff
	};

    //------
    //CEvent
    //------

    class CEvent
    {
    public:
        _forceinline CEvent(uint32 categories)
            : m_Categories(categories) {}
        virtual ~CEvent() = default;
        
		virtual EventType GetType() const = 0;
		_forceinline bool IsInCategory(EEventCategory category)
		{ 
			return (m_Categories & category); 
		}
        
		_forceinline uint32 GetCategoryFlags() const           
		{ 
			return m_Categories; 
		}
    private:
        uint32 m_Categories;
	private:
		_forceinline static constexpr EventType GetStaticType()
		{ 
			constexpr EventType hash = StringHash("Event");
			return hash; 
		}
    };
}
