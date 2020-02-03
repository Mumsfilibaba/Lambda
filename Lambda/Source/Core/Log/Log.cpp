#include "LambdaPch.h"

#include "Core/Engine.h"

#include "Core/Log/Log.h"
#include "Core/Debug/IConsoleOutput.h"

#include "Platform/Platform.h"

#include <stdio.h>

namespace Lambda
{
    CLog::CLog()
        : m_bEnabled(true)
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
            
            IConsoleOutput* pOutput = g_Engine.GetErrorOutput();
            if (pOutput)
            {
                if (verbosity == ELogVerbosity::LOG_VERBOSITY_INFO)
                {
                    pOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_GREEN);
                    pOutput->Write("[INFO] ");
                }
                else if (verbosity == ELogVerbosity::LOG_VERBOSITY_WARNING)
                {
                    pOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_YELLOW);
                    pOutput->Write("[WARNING] ");
                }
                else if (verbosity == ELogVerbosity::LOG_VERBOSITY_ERROR)
                {
                    pOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_RED);
                    pOutput->Write("[ERROR] ");
                }

                pOutput->Write(buffer);
                pOutput->Write("\n");
                
                pOutput->SetTextColor(EConsoleColor::CONSOLE_COLOR_WHITE);
            }
            
            Platform::OutputDebugString(buffer);
#if defined(LAMBDA_PLAT_WINDOWS)
            Platform::OutputDebugString("\n");
#endif
        }
    }
}
