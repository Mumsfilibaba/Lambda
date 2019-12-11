#pragma once
#include "CMouseState.h"
#include <glm/glm.hpp>

namespace Lambda
{
    //----------------
    //IMouseController
    //----------------

    class IMouseController
    {
    public:
        LAMBDA_INTERFACE(IMouseController);
        
        virtual void SetPosition(const glm::ivec2& position) = 0;
        virtual glm::ivec2 GetPosition() const = 0;
        virtual CMouseState GetMouseState() const = 0;
    };
}
