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
        static bool IsMouseButtonPressed(EMouseButton button);
        static bool IsMouseButtonUp(EMouseButton button);
        static bool IsMouseButtonDown(EMouseButton button);
        static void SetMousePosition(const glm::ivec2& position);
        static glm::ivec2 GetMousePosition();
        static const CMouseState& GetMouseState();
    private:
        static void Update();
    private:
        static CMouseState s_CurrentState;
        static CMouseState s_LastState;
    };
}
