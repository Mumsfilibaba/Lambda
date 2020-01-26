#include "LambdaPch.h"

#include "Core/Log/Log.h"

namespace Lambda
{
    CLog::CLog()
        : m_bEnabled(false)
    {
    }

    void CLog::Enable(bool bEnable)
    {
        m_bEnabled = bEnable;
    }

    bool CLog::IsEnabled() const
    {
        return m_bEnabled;
    }

    void CLog::Print(ELogVerbosity verbosity, const char* pFormat, ...)
    {
        va_list args;
        va_start(args, pFormat);
        CLog::PrintV(verbosity, pFormat, args);
        va_end(args);
    }

    void CLog::PrintV(ELogVerbosity verbosity, const char* pFormat, va_list args)
    {
        //TODO: Print to console
    }
}
