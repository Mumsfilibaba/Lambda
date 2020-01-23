#include "LambdaPch.h"

#ifdef LAMBDA_PLAT_MACOS
    #include "Platform/macOS/MacApplication.h"

namespace Lambda
{
    void CMacApplication::Init()
    {
    }
    
    void CMacApplication::PollEvents()
    {
    }
}
#endif
