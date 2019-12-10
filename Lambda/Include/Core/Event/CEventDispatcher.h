#pragma once
#include "CEvent.h"

namespace Lambda
{
    //----------------
    //CEventDispatcher
    //----------------

    class LAMBDA_API CEventDispatcher
    {
    public:
        CEventDispatcher() = default;
        ~CEventDispatcher() = default;
        
        LAMBDA_NO_COPY(CEventDispatcher);
        
        //Dispatch event to another function
        template <typename O, typename T>
        _forceinline bool Dispatch(O* pObjectRef, bool(O::*objectCallbackFunc)(const T&), const CEvent& event)
        {
            if (event.GetType() == T::GetStaticType())
            {
                if ((pObjectRef->*objectCallbackFunc)(static_cast<const T&>(event)))
                    return true;
            }

            return false;
        }
    };
}
