#pragma once
#include "Defines.h"
#include "Types.h"

namespace Lambda
{
    //---------------
    //EMessageBoxType
    //---------------
    enum class EMessageBoxType : uint32
    {   
        MESSAGE_BOX_TYPE_UNKNOWN    = 0,
        MESSAGE_BOX_TYPE_ERROR      = 1,
    };

    //---------
    //CPlatform
    //---------
    class CPlatform
    {
    public:
        LAMBDA_STATIC_CLASS(CPlatform);

        static void MessageBox(const char* pCaption, const char* pText, EMessageBoxType type);
    };
}