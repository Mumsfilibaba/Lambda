#pragma once
#include "Core/LambdaDefines.h"
#include "Core/LambdaTypes.h"

namespace Lambda
{
    //--------------
    //AssertManager
    //--------------

    class AssertManager
    {
    public:
        /*////////////////////////////////////////////////////////////////////////////////////////////////*/
        inline AssertManager()
            : m_bShowDialogOnAssert(true),
            m_bWriteConsoleOnAssert(true)
        {
        }
        
        ~AssertManager() = default;
        LAMBDA_NO_COPY(AssertManager);
        
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
