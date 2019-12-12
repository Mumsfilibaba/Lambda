#pragma once
#include "CMouseState.h"
#include <glm/glm.hpp>

namespace Lambda
{
    //------
    //CMouse
    //------

    class LAMBDA_API CMouse final
    {
        friend class CEngine;
    public:
        LAMBDA_STATIC_CLASS(CMouse);
        
        // Difference between mousebutton-down and -pressed is that pressed only checks the current frame's state
        static bool IsButtonPressed(EMouseButton button);
        static bool IsButtonUp(EMouseButton button);
        static bool IsButtonDown(EMouseButton button);
        static void SetPosition(const Point& position);
        static Point GetPosition();
        static const CMouseState& GetState();
    private:
        static void UpdateState();
    private:
        static CMouseState s_CurrentState;
        static CMouseState s_LastState;
    };
}
