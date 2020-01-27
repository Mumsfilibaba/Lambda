#include "LambdaPch.h"

#include "Core/Log/Log.h"
#include "Core/Debug/IConsoleOutput.h"

#include "Platform/Platform.h"

#include <stdio.h>

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
        if (m_bEnabled)
        {
            constexpr uint32 MAX_MESSAGE_LENGTH = 512;
            static char buffer[MAX_MESSAGE_LENGTH];
            
            memset(buffer, 0, sizeof(buffer));
            vsnprintf(buffer, MAX_MESSAGE_LENGTH, pFormat, args);
            
            if (g_pConsoleOutput)
            {
                if (verbosity == ELogVerbosity::LOG_VERBOSITY_INFO)
                {
                    g_pConsoleOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_GREEN);
                    g_pConsoleOutput->Write("[INFO] ");
                }
                else if (verbosity == ELogVerbosity::LOG_VERBOSITY_WARNING)
                {
                    g_pConsoleOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_YELLOW);
                    g_pConsoleOutput->Write("[WARNING] ");
                }
                else if (verbosity == ELogVerbosity::LOG_VERBOSITY_ERROR)
                {
                    g_pConsoleOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_RED);
                    g_pConsoleOutput->Write("[ERROR] ");
                }

                g_pConsoleOutput->Write(buffer);
                g_pConsoleOutput->Write("\n");
                
                g_pConsoleOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_WHITE);
            }
            
            Platform::OutputDebugString(buffer);
#if defined(LAMBDA_PLAT_WINDOWS)
            Platform::OutputDebugString("\n");
#endif
        }
    }
}
