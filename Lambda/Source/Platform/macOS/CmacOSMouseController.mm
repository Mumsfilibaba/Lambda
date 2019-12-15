#import "CmacOSMouseController.h"
#if defined(LAMBDA_PLAT_MACOS)

namespace Lambda
{
    //---------------------
    //CmacOSMouseController
    //---------------------
    
    CmacOSMouseController::CmacOSMouseController(CEnvironment* pEnvironment)
        : CMouseControllerBase(pEnvironment)
    {
    }
    
    
    Point CmacOSMouseController::GetPosition() const
    {
        return Point();
    }

    
    void CmacOSMouseController::SetPosition(const Point& position)
    {
    }

    
    void CmacOSMouseController::SetMouseVisisble(bool)
    {
    }

    
    bool CmacOSMouseController::IsMouseVisible() const
    {
        return false;
    }
}

#endif
