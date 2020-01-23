#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/macOS/MacMisc.h"

namespace Lambda
{
    void CMacMisc::MessageBox(const char* pCaption, const char* pText, uint32 type)
    {
    }

    void CMacMisc::DebuggerOutput(const char *pMessage)
    {
    }
}
#endif
