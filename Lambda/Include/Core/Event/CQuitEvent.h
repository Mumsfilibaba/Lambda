#pragma once
#include "CEvent.h"

namespace Lambda
{
    //----------
    //CQuitEvent
    //----------

    class CQuitEvent : public CEvent
    {
    public:
        _forceinline CQuitEvent(uint32 exitCode)
            : CEvent(EVENT_CATEGORY_ENVIRONMENT), m_ExitCode(exitCode) {}
        virtual ~CQuitEvent() = default;
        
        LAMBDA_DECLARE_EVENT_TYPE(CQuitEvent);
        
        _forceinline uint32 GetExitCode() const
        {
            return m_ExitCode;
        }
    private:
        uint32 m_ExitCode;
    };
}
