#pragma once
#include "LambdaCore.h"
#include "CMouseState.h"
#include <glm/glm.hpp>

namespace Lambda
{
    //------
    //CMouse
    //------

    class LAMBDA_API CMouse final
    {
    public:
        LAMBDA_STATIC_CLASS(CMouse);
        
        static bool IsMouseButtonDown();
        static bool IsMouseButtonUp();
        // Difference between mousebutton-down and -pressed is that pressed only checks the current frame's state
        static bool IsMouseButtonPressed();
        static const void SetMousePosition(const glm::ivec2& position);
        static const glm::ivec2& GetMousePosition();
        static const CMouseState& GetMouseState();
    };
}
