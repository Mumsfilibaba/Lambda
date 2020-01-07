#pragma once
#include "LambdaDefines.h"
#include "LambdaTypes.h"

namespace Lambda
{
    //-----------------
    //CAssertionManager
    //-----------------
    class CAssertionManager
    {
    public:
        inline CAssertionManager()
            : m_bShowDialogOnAssert(true),
            m_bWriteConsoleOnAssert(true)
        {
        }
        
        ~CAssertionManager() = default;
        
        LAMBDA_NO_COPY(CAssertionManager);
        
        inline void SetShowDialogOnAssert(bool bShowDialogOnAssert)
        {
            m_bShowDialogOnAssert = bShowDialogOnAssert;
        }
        
        inline void SetWriteConsoleOnAssert(bool bWriteConsoleOnAssert)
        {
            m_bWriteConsoleOnAssert = bWriteConsoleOnAssert;
        }
        
        inline bool ShowDialogOnAssert() const
        {
            return m_bShowDialogOnAssert;
        }
        
        inline bool WriteConsoleOnAssert() const
        {
            return m_bWriteConsoleOnAssert;
        }
    private:
        bool m_bShowDialogOnAssert;
        bool m_bWriteConsoleOnAssert;
    };
}
