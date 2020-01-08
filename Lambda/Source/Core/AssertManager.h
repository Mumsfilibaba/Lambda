#pragma once
#include "Core/LambdaDefines.h"
#include "Core/LambdaTypes.h"

namespace Lambda
{
    //--------------
    //CAssertManager
    //--------------
    class CAssertManager
    {
    public:
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        inline CAssertManager()
            : m_bShowDialogOnAssert(true),
            m_bWriteConsoleOnAssert(true)
        {
        }
        
        ~CAssertManager() = default;
        LAMBDA_NO_COPY(CAssertManager);
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        inline void SetShowDialogOnAssert(bool bShowDialogOnAssert)
        {
            m_bShowDialogOnAssert = bShowDialogOnAssert;
        }
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        inline void SetWriteConsoleOnAssert(bool bWriteConsoleOnAssert)
        {
            m_bWriteConsoleOnAssert = bWriteConsoleOnAssert;
        }
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        inline bool ShowDialogOnAssert() const
        {
            return m_bShowDialogOnAssert;
        }
        
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        inline bool WriteConsoleOnAssert() const
        {
            return m_bWriteConsoleOnAssert;
        }
    private:
        bool m_bShowDialogOnAssert;
        bool m_bWriteConsoleOnAssert;
    };
}
